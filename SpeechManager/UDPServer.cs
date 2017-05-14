using System;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Threading;


namespace SpeechManager
{
    public class UDPEventArgs : EventArgs
    {
        public UDPEventArgs(string s)
        {
            msg = s;
        }

        public string msg { get; set; }
    }

    public class UDPServer
    {
        public event UDPReceiveEventDelegate UDPReceiveHappened;

        private volatile bool is_stop_requested = false;
        private volatile bool is_socket_error = false;
        private volatile bool is_first_rx_ok = false;

        private UdpClient udpRxClient;
        private UdpClient udpTxClient;
        private IPEndPoint rxEP;
        private IPEndPoint txEP;

        private readonly Mutex m = new Mutex();
        private volatile bool is_response_new = false;
        private string s_response = "";

        protected virtual void OnUDPReceive(UDPEventArgs e)
        {
            if (UDPReceiveHappened != null)
            {
                UDPReceiveHappened(e);
            }
        }

        public void Init(string s, int rxPort, int txPort)
        {
            try
            {
                udpRxClient = new UdpClient();
                rxEP = new IPEndPoint(IPAddress.Parse(s), rxPort);
                udpRxClient.ExclusiveAddressUse = false;
                udpRxClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                udpRxClient.Client.Bind(rxEP);
                udpRxClient.Client.ReceiveTimeout = 500;

                udpTxClient = new UdpClient();
                txEP = new IPEndPoint(IPAddress.Parse(s), txPort);
                udpTxClient.ExclusiveAddressUse = false;
                udpTxClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                udpTxClient.Client.Bind(rxEP);
            }
            catch (SocketException)
            {
                is_socket_error = true;
            }
        }

        public void SendMsg(string s)
        {
            ///////////////////////
            m.WaitOne();
            s_response = s;
            is_response_new = true;
            m.ReleaseMutex();
            ///////////////////////
        }

        public bool IsOK()
        {
            return !is_socket_error;
        }

        public void Stop()
        {
            is_stop_requested = true;
        }

        public void ThreadProcRX()
        {
            while (!is_stop_requested && !is_socket_error)
            {
                try
                {
                    byte[] buffer = udpRxClient.Receive(ref rxEP);
                    if (buffer.Length > 0)
                    {
                        // set flag that socket has received something
                        // Windows has issues if TX attempted before RX occurs
                        is_first_rx_ok = true;
                        string s = System.Text.Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        OnUDPReceive(new UDPEventArgs(s));
                    }
                }
                catch (SocketException ex)
                {
                    if (ex.ErrorCode != (int) SocketError.TimedOut)
                    {
                        is_socket_error = true;
                    }
                }
            }

            if (!udpRxClient.Equals(null))
            {
                udpRxClient.Close();
            }
        }

        public void ThreadProcTX()
        {
            while (!is_stop_requested && !is_socket_error)
            {
                bool is_ok;
                string s;

                ////////////////////////
                m.WaitOne();
                is_ok = is_response_new;
                is_response_new = false;
                s = s_response;
                m.ReleaseMutex();
                ////////////////////////

                // only send a string if it has been newly stuff in buffer
                // and if an RX has already occurred, otherwise Windows
                // will throw an error in the RX loop
                if (is_ok && is_first_rx_ok)
                {
                    byte[] buffer = Encoding.ASCII.GetBytes(s);
                    udpTxClient.Send(buffer, buffer.Length, txEP);
                }

                Thread.Sleep(20);
            }

            if (!udpTxClient.Equals(null))
            {
                udpTxClient.Close();
            }
        }
    }
}
