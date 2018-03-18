using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SpeechManager
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            string sipaddr = "127.0.0.1";
            int rxPort = 60000;
            int txPort = 60001;
            string sculture = "en-US";

            if (args.Length >= 1)
            {
                sipaddr = args[0];
            }

            if (args.Length >= 2)
            {
                rxPort = Convert.ToInt32(args[1]);
            }

            if (args.Length >= 3)
            {
                txPort = Convert.ToInt32(args[2]);
            }

            if (args.Length >= 4)
            {
                sculture = args[3];
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1(sipaddr, rxPort, txPort, sculture));
        }
    }
}
