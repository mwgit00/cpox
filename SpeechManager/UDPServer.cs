using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Net;
using System.Net.Sockets;


namespace SpeechManager
{
    public class UDPServer
    {
        private volatile bool is_stop_requested = false;
        private volatile bool is_socket_error = false;

        private const int listenPort = 60000;
        private UdpClient listener;
        private IPEndPoint groupEP;

        public void Init()
        {
            listener = new UdpClient(listenPort);
            groupEP = new IPEndPoint(IPAddress.Any, listenPort);
            listener.Client.ReceiveTimeout = 500;
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
                        Console.WriteLine(s);
                    }
                }
                catch (SocketException ex)
                {
                    if (ex.ErrorCode != (int) SocketError.TimedOut)
                    {
                        Console.WriteLine("Dead.");
                        is_socket_error = true;
                    }
                }
            }
            listener.Close();
        }
    }
}
