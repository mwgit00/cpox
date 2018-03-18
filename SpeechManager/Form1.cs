using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Collections.Concurrent;
using System.Speech.Recognition;
using System.Speech.Synthesis;
using System.Threading;
using System.Media;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Globalization;


namespace SpeechManager
{
    public delegate void UDPReceiveEventDelegate(UDPEventArgs e);

    public partial class Form1 : Form
    {
        private string s_partner_ip = "127.0.0.1";
        private int listenPort = 60000;
        private int answerPort = 60001;
        private string s_culture = "en-US";

        private int rec_time_sec = 20;
        private const int INTERVAL_MS = 100;
        private const string s_tts_ack = "tts 1";
        private const string s_rec_ack = "rec ";

        private ConcurrentQueue<string> xmsgq =
            new ConcurrentQueue<string>();

        private SpeechManager.UDPServer theServer = new SpeechManager.UDPServer();
        private Thread T_RX;
        private Thread T_TX;

        private bool is_last_cmd_done = true;
        private bool is_rec_tmr_running = false;
        private bool is_wav_tmr_running = false;

        private int t_rec_ct = 0;
        private int t_wav_ct = 0;
        private int t_wav_ct_max = 0;

        private SpeechRecognitionEngine recognizer;
        private SpeechSynthesizer synth = new SpeechSynthesizer();
        private string phrase = "this is a test";
        private int word_ct = 0;
        private double rec_conf = 0.0;
        private bool is_rec_valid = false;

        private SoundPlayer sounder = new SoundPlayer();

        public Form1()
        {
            InitializeComponent();
        }

        public Form1(string sipaddr, int rxPort, int txPort, string sculture)
        {
            s_partner_ip = sipaddr;
            listenPort = rxPort;
            answerPort = txPort;
            s_culture = sculture;
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // set up sound player stuff

            sounder.LoadCompleted +=
                new AsyncCompletedEventHandler(snd_LoadCompleted);
            
            // set up all the speech synthesis stuff

            synth.SelectVoiceByHints(VoiceGender.Female);
            synth.SetOutputToDefaultAudioDevice();

            synth.SpeakStarted +=
                new EventHandler<SpeakStartedEventArgs>(
                    tts_SpeakStarted);
            synth.SpeakCompleted +=
                new EventHandler<SpeakCompletedEventArgs>(
                    tts_SpeakCompleted);

            // set up all the recognition stuff

            try
            {
                recognizer =
                    new SpeechRecognitionEngine(
                        new System.Globalization.CultureInfo(s_culture));

                recognizer.SpeechRecognized +=
                    new EventHandler<SpeechRecognizedEventArgs>(
                        sre_SpeechRecognized);
                recognizer.RecognizeCompleted +=
                    new EventHandler<RecognizeCompletedEventArgs>(
                        sre_RecognizeCompletedHandler);
                recognizer.SpeechRecognitionRejected +=
                    new EventHandler<SpeechRecognitionRejectedEventArgs>(
                        sre_SpeechRecognitionRejectedHandler);
                recognizer.SpeechHypothesized +=
                    new EventHandler<SpeechHypothesizedEventArgs>(
                        sre_SpeechHypothesized);
                recognizer.SpeechDetected +=
                    new EventHandler<SpeechDetectedEventArgs>(
                        sre_SpeechDetected);

                recognizer.AudioSignalProblemOccurred +=
                    new EventHandler<AudioSignalProblemOccurredEventArgs>(
                        sre_AudioSignalProblemOccurred);
                recognizer.AudioLevelUpdated +=
                    new EventHandler<AudioLevelUpdatedEventArgs>(
                        sre_AudioLevelUpdated);
                recognizer.AudioStateChanged +=
                    new EventHandler<AudioStateChangedEventArgs>(
                        sre_AudioStateChanged);

                // hope this magically picks whatever audio input is connected

                recognizer.SetInputToDefaultAudioDevice();
            }
            catch (Exception ex)
            {
                textBoxUI.AppendText(
                    "Error initializing speech recognition engine:" +
                    Environment.NewLine + ex.Message + Environment.NewLine);
            }

            loadSinglePhraseGrammar(phrase);

            textBoxUI.VisibleChanged += (x_sender, x_e) =>
            {
                if (textBoxUI.Visible)
                {
                    textBoxUI.SelectionStart = textBoxUI.TextLength;
                    textBoxUI.ScrollToCaret();
                }
            };

            // echo command line parameters

            textBoxUI.AppendText("Partner: " + s_partner_ip + Environment.NewLine);
            textBoxUI.AppendText("RX Port: " + listenPort.ToString() + Environment.NewLine);
            textBoxUI.AppendText("TX Port: " + answerPort.ToString() + Environment.NewLine);
            textBoxUI.AppendText("Culture: " + s_culture + Environment.NewLine);

            // start the UDP command-response server

            theServer.Init(s_partner_ip, listenPort, answerPort);
            if (theServer.IsOK())
            {
                theServer.UDPReceiveHappened += new UDPReceiveEventDelegate(server_UDPReceived);

                T_RX = new Thread(new ThreadStart(theServer.ThreadProcRX));
                T_RX.IsBackground = true;
                T_RX.Start();

                T_TX = new Thread(new ThreadStart(theServer.ThreadProcTX));
                T_TX.IsBackground = true;
                T_TX.Start();

                textBoxUI.AppendText("UDP Server initialized." + Environment.NewLine);
                labelIsServerOK.BackColor = Color.Green;
            }
            else
            {
                textBoxUI.AppendText("UDP Server failed to initialize." + Environment.NewLine);
                labelIsServerOK.BackColor = Color.Red;
            }

            // set up GUI

            cmStatus.Items.Clear();
            cmStatus.Items.Add("Select All");
            cmStatus.Items.Add("Copy");
            cmStatus.Items.Add("Clear");
            cmStatus.ItemClicked += new ToolStripItemClickedEventHandler(cmStatus_ItemClicked);

            timerEvent.Interval = INTERVAL_MS;
            timerEvent.Start();

            labelIsSpeaking.BackColor = Color.Gray;
            labelIsRecognizing.BackColor = Color.Gray;
            textBoxStatus.BackColor = Color.LightBlue;
            textBoxPhrase.Text = phrase;
            textBoxUI.AppendText("Ready." + Environment.NewLine);
        }

        void snd_LoadCompleted(object sender, AsyncCompletedEventArgs e)
        {
            if (sounder.IsLoadCompleted)
            {
                try
                {
                    // begin playing WAV and set countdown timer
                    // set state showing WAV is in progress
                    this.sounder.Play();
                    t_wav_ct = t_wav_ct_max;
                    is_wav_tmr_running = true;
                }
                catch (Exception ex)
                {
                    textBoxUI.AppendText(
                        "Error playing WAV file:" + 
                        Environment.NewLine + ex.Message + Environment.NewLine);
                }
            }
        }

        void sre_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            if (checkBoxShowHypothesis.Checked == true)
            {
                textBoxUI.AppendText(Environment.NewLine);
            }

            is_rec_valid = true;
            rec_conf = e.Result.Confidence;
        }

        void sre_AudioSignalProblemOccurred(object sender, AudioSignalProblemOccurredEventArgs e)
        {
            if (checkBoxShowAudioProblems.Checked == true)
            {
                // show all the gory details if desired

                StringBuilder details = new StringBuilder();

                details.AppendLine("Audio signal problem information:");
                details.AppendFormat(
                  " Audio level:               {0}" + Environment.NewLine +
                  " Audio position:            {1}" + Environment.NewLine +
                  " Audio signal problem:      {2}" + Environment.NewLine +
                  " Recognizer audio position: {3}" + Environment.NewLine,
                  e.AudioLevel,
                  e.AudioPosition,
                  e.AudioSignalProblem,
                  e.RecognizerAudioPosition);

                textBoxUI.AppendText(details.ToString());
            }
        }

        void sre_AudioLevelUpdated(object sender, AudioLevelUpdatedEventArgs e)
        {
            // progress bar is a simple audio meter
            audioLevelBar.Value = e.AudioLevel;
        }

        void sre_AudioStateChanged(object sender, AudioStateChangedEventArgs e)
        {
            AudioState newState = e.AudioState;
            if (newState == AudioState.Silence)
            {
                textBoxStatus.BackColor = Color.LightGreen;
                textBoxStatus.Text = "Silence";
            }
            else if (newState == AudioState.Speech)
            {
                textBoxStatus.BackColor = Color.Yellow;
                textBoxStatus.Text = "Speech";
            }
            else if (newState == AudioState.Stopped)
            {
                textBoxStatus.BackColor = Color.LightBlue;
                textBoxStatus.Text = "Idle";
                audioLevelBar.Value = 0;
            }
        }

        void sre_SpeechRecognitionRejectedHandler(
          object sender, SpeechRecognitionRejectedEventArgs e)
        {
            is_rec_valid = true;
            rec_conf = 0.0;
        }

        void sre_RecognizeCompletedHandler(object sender, RecognizeCompletedEventArgs e)
        {
            labelIsRecognizing.BackColor = Color.Gray;
            is_last_cmd_done = true;
            is_rec_tmr_running = false;
            textBoxRecTimer.Text = "0";

            buttonRecognize.Enabled = true;
            buttonCancel.Enabled = false;
            buttonSpeak.Enabled = true;

            string sval = "0";
            string sresult = "FAIL: ";
            if (is_rec_valid)
            {
                double thr = (double)numericUpDownMinScore.Value;
                string sconf = rec_conf.ToString("0.000");
                if (rec_conf > (thr / 100.0))
                {
                    sresult = "PASS: ";
                    sval = "1";
                }
                textBoxUI.AppendText(sresult + sconf + Environment.NewLine);
            }

            theServer.SendMsg(s_rec_ack + sval);
        }

        void sre_SpeechHypothesized(object sender, SpeechHypothesizedEventArgs e)
        {
            if (checkBoxShowHypothesis.Checked == true)
            {
                for (int k = word_ct; k < e.Result.Words.Count; k++)
                {
                    string sk = e.Result.Words[k].Text;
                    float f = e.Result.Words[k].Confidence;
                    textBoxUI.AppendText(sk + "," + f.ToString() + "? ");
                }
                word_ct = e.Result.Words.Count;
            }
        }

        private void buttonRecognize_Click(object sender, EventArgs e)
        {
            // change state of related buttons
            // recognize the loaded phrase
            buttonRecognize.Enabled = false;
            buttonCancel.Enabled = true;
            buttonSpeak.Enabled = false;
            start_recognition();
        }

        private void checkBoxShowAudioProblems_CheckedChanged(object sender, EventArgs e)
        {
            // placeholder
        }

        private void sre_SpeechDetected(object sender, SpeechDetectedEventArgs e)
        {
            // placeholder
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            buttonCancel.Enabled = false;
            cancel_speech_actions();
        }

        private void buttonSpeak_Click(object sender, EventArgs e)
        {
            buttonRecognize.Enabled = false;
            buttonSpeak.Enabled = false;
            start_speaking(phrase);
        }

        void tts_SpeakStarted(object sender, SpeakStartedEventArgs e)
        {
            labelIsSpeaking.BackColor = Color.LightGreen;
        }

        void tts_SpeakCompleted(object sender, SpeakCompletedEventArgs e)
        {
            buttonRecognize.Enabled = true;
            buttonSpeak.Enabled = true;
            labelIsSpeaking.BackColor = Color.Gray;
            theServer.SendMsg(s_tts_ack);
            is_last_cmd_done = true;
        }

        private void checkBoxEditPhrase_CheckedChanged(object sender, EventArgs e)
        {
            textBoxPhrase.Enabled = checkBoxEditPhrase.Checked;
            if (checkBoxEditPhrase.Checked == false)
            {
                // turning off editing of phrase (unclicking checkbox)
                // will apply it as new grammar

                recognizer.UnloadAllGrammars();
                loadSinglePhraseGrammar(textBoxPhrase.Text);
                textBoxUI.AppendText("New phrase applied." + Environment.NewLine);
            }
        }

        private void loadSinglePhraseGrammar(string newPhrase)
        {
            phrase = newPhrase;

            try
            {
                // create a simple one-phrase grammar
                Choices phraseChoice = new Choices();
                phraseChoice.Add(new string[] { newPhrase });

                // create a GrammarBuilder and append the Choices object
                GrammarBuilder gb = new GrammarBuilder();
                gb.Culture = new System.Globalization.CultureInfo(s_culture);
                gb.Append(phraseChoice);

                // then create the Grammar instance and load it
                // into the speech recognition engine.
                Grammar g = new Grammar(gb);
                recognizer.LoadGrammar(g);
            }
            catch (Exception ex)
            {
                textBoxUI.AppendText(
                    "Error initializing grammar:" +
                    Environment.NewLine + ex.Message + Environment.NewLine);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // stop any timer events
            // then assert server stop flag and wait for it to end
            timerEvent.Stop();
            theServer.Stop();
            if (!T_RX.Equals(null))
            {
                if (T_RX.IsAlive)
                {
                    T_RX.Join();
                }
            }
        }

        private void server_UDPReceived(UDPEventArgs e)
        {
            // using thread-safe concurrent queue
            // so no invoke is necessary

            if (e.msg == "cancel")
            {
                // do a dummy loop to drain the queue
                // prior to queueing the cancel command below
                string smsg;
                while (xmsgq.TryDequeue(out smsg))
                {
                    // do nothing
                }
            }

            // queue the command
            xmsgq.Enqueue(e.msg);
        }

        private void timerEvent_Tick(object sender, EventArgs e)
        {
            string s = "";
            bool cmdFlag = false;
            bool is_cancel = false;

            // color the server status box
            // it will go red if there was a socket error
            if (theServer.IsOK())
            {
                labelIsServerOK.BackColor = Color.Green;
            }
            else
            {
                labelIsServerOK.BackColor = Color.Red;

                if (theServer.error_message_rx.Length > 0)
                {
                    // show RX error message once then clear it so it doesn't keep getting printed
                    textBoxUI.AppendText(theServer.error_message_rx + Environment.NewLine);
                    theServer.error_message_rx = "";
                }

                if (theServer.error_message_tx.Length > 0)
                {
                    // show TX error message once then clear it so it doesn't keep getting printed
                    textBoxUI.AppendText(theServer.error_message_tx + Environment.NewLine);
                    theServer.error_message_tx = "";
                }
            }

            // handle recognition timeout
            if (is_rec_tmr_running)
            {
                t_rec_ct += INTERVAL_MS;
                int x = t_rec_ct / 1000;
                int t_left = rec_time_sec - x;
                textBoxRecTimer.Text = t_left.ToString();
                if (t_left == 0)
                {
                    is_rec_tmr_running = false;
                    textBoxUI.AppendText("Recognition timeout" + Environment.NewLine);
                    cancel_speech_actions();
                }
            }

            // handle wav player timeout
            if (is_wav_tmr_running)
            {
                t_wav_ct--;
                if (t_wav_ct <= 0)
                {
                    is_last_cmd_done = true;
                    is_wav_tmr_running = false;
                    buttonRecognize.Enabled = true;
                    buttonSpeak.Enabled = true;
                    labelIsSpeaking.BackColor = Color.Gray;
                    theServer.SendMsg(s_tts_ack);
                }
            }

            // peek to see if there is a cancel command
            if (xmsgq.TryPeek(out s))
            {
                is_cancel = (s == "cancel");
            }

            // can try to pop a command if last command is done
            // or if a cancel is the next command
            // a cancel can always be handled
            if (is_last_cmd_done || is_cancel)
            {
                cmdFlag = xmsgq.TryDequeue(out s);
            }

            // if cmd found then execute
            if (cmdFlag)
            {
                string[] sarray = s.Split(' ');
                int n_tokens = sarray.Length;
                string firstElem = sarray.First();
                string restOfArray = string.Join(" ", sarray.Skip(1));

                if (firstElem == "wav")
                {
                    textBoxWAV.Text = restOfArray;
                    buttonWAV.Enabled = true;
                    start_wav_player(restOfArray);
                }
                else if (firstElem == "say")
                {
                    if (restOfArray.Length > 0)
                    {
                        // say whatever is RX'ed
                        start_speaking(restOfArray);
                    }
                    else
                    {
                        textBoxUI.AppendText("Empty 'say' command." + Environment.NewLine);
                    }
                }
                else if (firstElem == "cancel")
                {
                    // textBoxUI.AppendText("Cancel!" + Environment.NewLine);
                    cancel_speech_actions();
                }
                else if (firstElem == "repeat")
                {
                    // repeat the loaded phrase
                    start_speaking(phrase);
                }
                else if (firstElem == "rec")
                {
                    // recognize the loaded phrase
                    start_recognition();
                }
                else if (firstElem == "load")
                {
                    if (restOfArray.Length > 0)
                    {
                        // load a new phrase
                        // immediate operation, no need to clear done flag
                        phrase = restOfArray;
                        textBoxPhrase.Text = phrase;
                        recognizer.UnloadAllGrammars();
                        loadSinglePhraseGrammar(phrase);
                    }
                    else
                    {
                        textBoxUI.AppendText("Empty 'load' command." + Environment.NewLine);
                    }
                }
                else
                {
                    textBoxUI.AppendText("Unrecognized command." + Environment.NewLine);
                }
            }
        }

        public void start_wav_player(string filepath)
        {
            bool is_wav_file = false;
            if (File.Exists(filepath))
            {
                if (Path.GetExtension(filepath) == ".wav")
                {
                    is_wav_file = true;
                }
            }

            if (is_wav_file)
            {
                // extract bit rate info directly from binary WAV header
                // and calculate duration in milliseconds
                byte[] TotalBytes = File.ReadAllBytes(filepath);
                int bitrate = (BitConverter.ToInt32(new[] { TotalBytes[28], TotalBytes[29], TotalBytes[30], TotalBytes[31] }, 0) * 8);
                double duration_ms = 1000.0 * ((TotalBytes.Length - 8) * 8) / ((double)bitrate);

                int nticks = (int)((duration_ms / ((double)INTERVAL_MS)) + 0.5) + 1;
                int nsecs = nticks * INTERVAL_MS;
                t_wav_ct_max = nticks;
                // textBoxUI.AppendText(filepath + ":  " + nsecs.ToString() + "ms" + Environment.NewLine);

                try
                {
                    this.sounder.SoundLocation = filepath;
                    this.sounder.LoadAsync();

                    buttonRecognize.Enabled = false;
                    buttonSpeak.Enabled = false;
                    labelIsSpeaking.BackColor = Color.LightGreen;
                    is_last_cmd_done = false;
                }
                catch (Exception ex)
                {
                    textBoxUI.AppendText(
                        "Error loading WAV file:" +
                        Environment.NewLine + ex.Message + Environment.NewLine);
                }
            }
            else
            {
                textBoxUI.AppendText("Invalid WAV file:  " + filepath + Environment.NewLine);
            }
        }

        public void start_recognition()
        {
            // clear done flag, rec done event will set it
            is_last_cmd_done = false;
            is_rec_tmr_running = true;
            is_rec_valid = false;
            word_ct = 0;
            t_rec_ct = 0;
            rec_time_sec = Decimal.ToInt32(numericUpDownRecTime.Value);
            textBoxRecTimer.Text = rec_time_sec.ToString();
            textBoxStatus.BackColor = Color.LightGreen;
            textBoxStatus.Text = "Silence";
            labelIsRecognizing.BackColor = Color.LightGreen;
            recognizer.RecognizeAsync();
        }

        public void start_speaking(string s)
        {
            // clear done flag, TTS done event will set it
            is_last_cmd_done = false;
            synth.SpeakAsync(s);
        }

        public void cancel_speech_actions()
        {
            // cancel any recognition, TTS, or WAV in progress
            // (setting WAV ct to 0 will force timeout)
            recognizer.RecognizeAsyncCancel();
            synth.SpeakAsyncCancelAll();
            t_wav_ct = 0;
        }

        private void buttonWAV_Click(object sender, EventArgs e)
        {
            start_wav_player(textBoxWAV.Text);
        }

        private void buttonOpenWAV_Click(object sender, EventArgs e)
        {
            openFileDialog1.FileName = "";
            openFileDialog1.Filter = "WAV files (*.wav)|*.wav";
            if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                textBoxWAV.Text = openFileDialog1.FileName;
                buttonWAV.Enabled = true;
            }
        }

        void cmStatus_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            ToolStripItem item = e.ClickedItem;
            if (item.Text == "Clear")
            {
                textBoxUI.Clear();
            }
            else if (item.Text == "Copy")
            {
                textBoxUI.Copy();
            }
            else if (item.Text == "Select All")
            {
                textBoxUI.SelectAll();
            }
        }
    }
}
