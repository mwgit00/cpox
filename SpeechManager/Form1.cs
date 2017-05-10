﻿using System;
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



namespace SpeechManager
{
    public delegate void UDPReceiveEventDelegate(UDPEventArgs e);

    public partial class Form1 : Form
    {
        private const int REC_TIME_SEC = 20;
        private const int INTERVAL_MS = 100;
        private const int COUNTDOWN_MS = REC_TIME_SEC * 1000;
        private const string s_tts_ack = "tts:0";
        private const string s_rec_ack = "rec:";

        private ConcurrentQueue<string> xmsgq =
            new ConcurrentQueue<string>();

        private SpeechManager.UDPServer theServer = new SpeechManager.UDPServer();
        private Thread T;

        private bool is_auto = false;
        private bool is_last_cmd_done = true;
        private bool is_rec_tmr_running = false;
        private int t_ct;

        public SpeechRecognitionEngine recognizer = new SpeechRecognitionEngine();
        public SpeechSynthesizer synth = new SpeechSynthesizer();
        public String phrase = "this is a test";
        public int word_ct = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
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

            loadSinglePhraseGrammar(phrase);

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

            // start the UDP command-response server

            theServer.Init();
            if (theServer.IsOK())
            {
                T = new Thread(new ThreadStart(theServer.ThreadProc));
                theServer.UDPReceiveHappened += new UDPReceiveEventDelegate(server_UDPReceived);
                T.IsBackground = true;
                T.Start();
                textBoxUI.AppendText("UDP Server initialized." + Environment.NewLine);
            }
            else
            {
                textBoxUI.AppendText("UDP Server failed to initialize." + Environment.NewLine);
            }

            // set up GUI

            timerEvent.Interval = INTERVAL_MS;
            timerEvent.Start();

            labelIsSpeaking.BackColor = Color.Gray;
            labelIsRecognizing.BackColor = Color.Gray;
            textBoxStatus.BackColor = Color.LightBlue;
            textBoxPhrase.Text = phrase;
            textBoxUI.AppendText("Ready!" + Environment.NewLine);
        }

        void sre_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            if (checkBoxShowHypothesis.Checked == true)
            {
                textBoxUI.AppendText(Environment.NewLine);
            }

            textBoxUI.AppendText("YAY! " + e.Result.Text + Environment.NewLine);
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
            textBoxUI.AppendText("Rejected" + Environment.NewLine);
#if FALSE
            Console.WriteLine(" In SpeechRecognitionRejectedHandler:");

            string grammarName = "<not available>";
            string resultText = "<not available>";
            if (e.Result != null)
            {
                if (e.Result.Grammar != null)
                {
                    grammarName = e.Result.Grammar.Name;
                }
                resultText = e.Result.Text;
            }

            Console.WriteLine(" - Grammar Name = {0}; Result Text = {1}",
              grammarName, resultText);
#endif
        }

        void sre_RecognizeCompletedHandler(object sender, RecognizeCompletedEventArgs e)
        {
            labelIsRecognizing.BackColor = Color.Gray;
            is_last_cmd_done = true;
            is_rec_tmr_running = false;
            textBoxRecTimer.Text = "0";
            if (!is_auto)
            {
                buttonRecognize.Enabled = true;
                buttonCancel.Enabled = false;
                buttonSpeak.Enabled = true;
                buttonAuto.Enabled = true;
            }
        }

        void sre_SpeechHypothesized(object sender, SpeechHypothesizedEventArgs e)
        {
            if (checkBoxShowHypothesis.Checked == true)
            {
                for (int k = word_ct; k < e.Result.Words.Count; k++)
                {
                    String sk = e.Result.Words[k].Text;
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
            buttonAuto.Enabled = false;
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
            if (is_auto)
            {
                textBoxUI.AppendText("Auto mode disabled" + Environment.NewLine);
                is_auto = false;
                buttonSpeak.Enabled = true;
                buttonRecognize.Enabled = true;
                buttonAuto.Enabled = true;
                buttonCancel.Enabled = false;
            }
            else
            {
                buttonCancel.Enabled = false;
                cancel_speech_actions();
            }
        }

        private void buttonSpeak_Click(object sender, EventArgs e)
        {
            if (!is_auto)
            {
                buttonRecognize.Enabled = false;
                buttonSpeak.Enabled = false;
                buttonAuto.Enabled = false;
            }
            start_speaking(phrase);
        }

        void tts_SpeakStarted(object sender, SpeakStartedEventArgs e)
        {
            labelIsSpeaking.BackColor = Color.LightGreen;
        }

        void tts_SpeakCompleted(object sender, SpeakCompletedEventArgs e)
        {
            if (!is_auto)
            {
                buttonRecognize.Enabled = true;
                buttonSpeak.Enabled = true;
                buttonAuto.Enabled = true;
            }
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

        private void loadSinglePhraseGrammar(String newPhrase)
        {
            phrase = newPhrase;
            
            // create a simple one-phrase grammar
            Choices phraseChoice = new Choices();
            phraseChoice.Add(new string[] {
                newPhrase,
            });

            // create a GrammarBuilder and append the Choices object
            GrammarBuilder gb = new GrammarBuilder();
            gb.Append(phraseChoice);

            // then create the Grammar instance and load it
            // into the speech recognition engine.
            Grammar g = new Grammar(gb);
            recognizer.LoadGrammar(g);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // stop any timer events
            // then assert server stop flag and wait for it to end
            timerEvent.Stop();
            theServer.Stop();
            if (!T.Equals(null))
            {
                if (T.IsAlive)
                {
                    T.Join();
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

            // handle recognition timeout
            if (is_rec_tmr_running)
            {
                t_ct += INTERVAL_MS;
                int x = t_ct / 1000;
                int t_left = REC_TIME_SEC - x;
                textBoxRecTimer.Text = t_left.ToString();
                if (t_left == 0)
                {
                    is_rec_tmr_running = false;
                    textBoxUI.AppendText("Recognition timeout" + Environment.NewLine);
                    cancel_speech_actions();
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

            // if cmd and auto mode then execute
            if (cmdFlag && is_auto)
            {
                String[] sarray = s.Split(' ');
                string firstElem = sarray.First();
                string restOfArray = string.Join(" ", sarray.Skip(1));

                if (firstElem == "say")
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
                        textBoxUI.AppendText("Empty phrase." + Environment.NewLine);
                    }
                }
                else
                {
                    textBoxUI.AppendText("Unrecognized command." + Environment.NewLine);
                }
            }
        }

        private void buttonAuto_Click(object sender, EventArgs e)
        {
            textBoxUI.AppendText("Auto mode enabled" + Environment.NewLine);
            is_auto = true;
            buttonSpeak.Enabled = false;
            buttonRecognize.Enabled = false;
            buttonAuto.Enabled = false;
            buttonCancel.Enabled = true;
        }

        private void buttonClear_Click(object sender, EventArgs e)
        {
            textBoxUI.Text = "";
        }

        public void start_recognition()
        {
            // clear done flag, rec done event will set it
            is_last_cmd_done = false;
            is_rec_tmr_running = true;
            word_ct = 0;
            t_ct = 0;
            textBoxRecTimer.Text = REC_TIME_SEC.ToString();
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
            theServer.SendMsg("speaking");
        }

        public void cancel_speech_actions()
        {
            recognizer.RecognizeAsyncCancel();
            synth.SpeakAsyncCancelAll();
        }
    }
}