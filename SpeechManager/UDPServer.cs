using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
        public event UDPReceiveEventDelegate StuffHappened;

        private volatile bool is_stop_requested = false;
        private volatile bool is_socket_error = false;

        private string s_partner_ip = "127.0.0.1";
        private const int listenPort = 60000;
        private const int answerPort = 60001;
        private UdpClient listener;
        private IPEndPoint groupEP;

        private readonly Mutex m = new Mutex();
        private volatile bool is_response_new = false;
        private string s_response = "";

        protected virtual void OnUDPReceive(UDPEventArgs e)
        {
            if (StuffHappened != null)
            {
                StuffHappened(e);
            }
        }

        public void Init(string s)
        {
            try
            {
                s_partner_ip = s;
                listener = new UdpClient(listenPort);
                groupEP = new IPEndPoint(IPAddress.Any, listenPort);
                listener.Client.ReceiveTimeout = 100;
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

        public void ThreadProc()
        {
            while (!is_stop_requested && !is_socket_error)
            {
                try
                {
                    byte[] buffer = listener.Receive(ref groupEP);
                    if (buffer.Length > 0)
                    {
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

                if (!is_socket_error)
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

                    if (is_ok)
                    {
                        Console.WriteLine(s);
                        byte[] buffer = Encoding.ASCII.GetBytes(s);
                        listener.Send(buffer, buffer.Length, s_partner_ip, answerPort);
                    }
                }
            }

            if (!listener.Equals(null))
            {
                listener.Close();
            }
        }
    }
}
