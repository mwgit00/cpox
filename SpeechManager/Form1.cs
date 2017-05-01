using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Speech.Recognition;
using System.Speech.Synthesis;

namespace netsr2
{
    public partial class Form1 : Form
    {
        public SpeechRecognitionEngine recognizer;
        public SpeechSynthesizer synth;
        public String phrase = "this is a test";
        public int word_ct = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // set up all the speech synthesis stuff

            synth = new SpeechSynthesizer();
            synth.SelectVoiceByHints(VoiceGender.Female);
            synth.SetOutputToDefaultAudioDevice();

            synth.SpeakStarted +=
                new EventHandler<SpeakStartedEventArgs>(
                    tts_SpeakStarted);
            synth.SpeakCompleted +=
                new EventHandler<SpeakCompletedEventArgs>(
                    tts_SpeakCompleted);

            // set up all the recognition stuff

            recognizer = new SpeechRecognitionEngine();
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

            // set up GUI

            textBoxPhrase.Text = phrase;
            textBoxUI.AppendText("Ready!" + Environment.NewLine);
        }

        void sre_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            if (checkBoxShowHypothesis.Checked == true)
            {
                textBoxUI.AppendText(Environment.NewLine);
            }

            textBoxUI.AppendText(e.Result.Text + Environment.NewLine);
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
                textBoxStatus.BackColor = Color.Green;
                textBoxStatus.Text = "Silence";
            }
            else if (newState == AudioState.Speech)
            {
                textBoxStatus.BackColor = Color.Yellow;
                textBoxStatus.Text = "Speech";
            }
            else if (newState == AudioState.Stopped)
            {
                textBoxStatus.BackColor = Color.CadetBlue;
                textBoxStatus.Text = "Stopped";
                buttonTest.Enabled = true;
                buttonCancel.Enabled = false;
                buttonSpeak.Enabled = true;
                audioLevelBar.Value = 0;
            }
        }

        void sre_SpeechRecognitionRejectedHandler(
          object sender, SpeechRecognitionRejectedEventArgs e)
        {
            textBoxUI.AppendText("rejected" + Environment.NewLine);
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

        // Handle the RecognizeCompleted event.
        void sre_RecognizeCompletedHandler(object sender, RecognizeCompletedEventArgs e)
        {
            textBoxUI.AppendText("completed" + Environment.NewLine);
        }

        void sre_SpeechHypothesized(object sender, SpeechHypothesizedEventArgs e)
        {
            if (checkBoxShowHypothesis.Checked == true)
            {
                for (int k = word_ct; k < e.Result.Words.Count; k++)
                {
                    String sk = e.Result.Words[k].Text;
                    textBoxUI.AppendText(sk + "? ");
                }
                word_ct = e.Result.Words.Count;
            }
        }

        private void buttonTest_Click(object sender, EventArgs e)
        {
            textBoxUI.AppendText("Recognizing..." + Environment.NewLine);
            word_ct = 0;
            buttonTest.Enabled = false;
            buttonCancel.Enabled = true;
            buttonSpeak.Enabled = false;
            textBoxStatus.BackColor = Color.Green;
            textBoxStatus.Text = "Silence";
            recognizer.RecognizeAsync();
        }

        private void checkBoxShowAudioProblems_CheckedChanged(object sender, EventArgs e)
        {
            //String s = checkBoxShowAudioProblems.Checked.ToString();
            //textBoxUI.AppendText("Show Audio Problems: " + s + Environment.NewLine);
        }

        private void sre_SpeechDetected(object sender, SpeechDetectedEventArgs e)
        {
#if FALSE
            Console.WriteLine("  Speech detected at AudioPosition = {0}", e.AudioPosition);
#endif
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            buttonCancel.Enabled = false;
            recognizer.RecognizeAsyncCancel();
            synth.SpeakAsyncCancelAll();
        }

        private void buttonSpeak_Click(object sender, EventArgs e)
        {
            synth.SpeakAsync(phrase);
            buttonTest.Enabled = false;
            buttonSpeak.Enabled = false;
        }

        void tts_SpeakStarted(object sender, SpeakStartedEventArgs e)
        {
            textBoxUI.AppendText("TTS started." + Environment.NewLine);
        }

        void tts_SpeakCompleted(object sender, SpeakCompletedEventArgs e)
        {
            buttonTest.Enabled = true;
            buttonSpeak.Enabled = true;
            textBoxUI.AppendText("TTS completed." + Environment.NewLine);
        }

        private void checkBoxEditPhrase_CheckedChanged(object sender, EventArgs e)
        {
            textBoxPhrase.Enabled = checkBoxEditPhrase.Checked;
            if (checkBoxEditPhrase.Checked == false)
            {
                // turning off editing of phrase
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
    }
}
