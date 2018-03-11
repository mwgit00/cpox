namespace SpeechManager
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.audioLevelBar = new System.Windows.Forms.ProgressBar();
            this.buttonRecognize = new System.Windows.Forms.Button();
            this.checkBoxShowAudioProblems = new System.Windows.Forms.CheckBox();
            this.checkBoxShowHypothesis = new System.Windows.Forms.CheckBox();
            this.textBoxPhrase = new System.Windows.Forms.TextBox();
            this.textBoxStatus = new System.Windows.Forms.TextBox();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.buttonSpeak = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.numericUpDownMinScore = new System.Windows.Forms.NumericUpDown();
            this.checkBoxEditPhrase = new System.Windows.Forms.CheckBox();
            this.timerEvent = new System.Windows.Forms.Timer(this.components);
            this.labelIsSpeaking = new System.Windows.Forms.Label();
            this.labelIsRecognizing = new System.Windows.Forms.Label();
            this.buttonClear = new System.Windows.Forms.Button();
            this.textBoxRecTimer = new System.Windows.Forms.TextBox();
            this.textBoxUI = new System.Windows.Forms.TextBox();
            this.labelIsServerOK = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.numericUpDownRecTime = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonWAV = new System.Windows.Forms.Button();
            this.textBoxWAV = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.buttonOpenWAV = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMinScore)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRecTime)).BeginInit();
            this.SuspendLayout();
            // 
            // audioLevelBar
            // 
            this.audioLevelBar.Location = new System.Drawing.Point(62, 61);
            this.audioLevelBar.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.audioLevelBar.Name = "audioLevelBar";
            this.audioLevelBar.Size = new System.Drawing.Size(75, 18);
            this.audioLevelBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.audioLevelBar.TabIndex = 1;
            // 
            // buttonRecognize
            // 
            this.buttonRecognize.Location = new System.Drawing.Point(289, 40);
            this.buttonRecognize.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.buttonRecognize.Name = "buttonRecognize";
            this.buttonRecognize.Size = new System.Drawing.Size(96, 25);
            this.buttonRecognize.TabIndex = 5;
            this.buttonRecognize.Text = "Recognize";
            this.buttonRecognize.UseVisualStyleBackColor = true;
            this.buttonRecognize.Click += new System.EventHandler(this.buttonRecognize_Click);
            // 
            // checkBoxShowAudioProblems
            // 
            this.checkBoxShowAudioProblems.AutoSize = true;
            this.checkBoxShowAudioProblems.Location = new System.Drawing.Point(4, 17);
            this.checkBoxShowAudioProblems.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.checkBoxShowAudioProblems.Name = "checkBoxShowAudioProblems";
            this.checkBoxShowAudioProblems.Size = new System.Drawing.Size(129, 17);
            this.checkBoxShowAudioProblems.TabIndex = 6;
            this.checkBoxShowAudioProblems.Text = "Show Audio Problems";
            this.checkBoxShowAudioProblems.UseVisualStyleBackColor = true;
            this.checkBoxShowAudioProblems.CheckedChanged += new System.EventHandler(this.checkBoxShowAudioProblems_CheckedChanged);
            // 
            // checkBoxShowHypothesis
            // 
            this.checkBoxShowHypothesis.AutoSize = true;
            this.checkBoxShowHypothesis.Location = new System.Drawing.Point(4, 39);
            this.checkBoxShowHypothesis.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.checkBoxShowHypothesis.Name = "checkBoxShowHypothesis";
            this.checkBoxShowHypothesis.Size = new System.Drawing.Size(108, 17);
            this.checkBoxShowHypothesis.TabIndex = 7;
            this.checkBoxShowHypothesis.Text = "Show Hypothesis";
            this.checkBoxShowHypothesis.UseVisualStyleBackColor = true;
            // 
            // textBoxPhrase
            // 
            this.textBoxPhrase.Enabled = false;
            this.textBoxPhrase.Location = new System.Drawing.Point(48, 153);
            this.textBoxPhrase.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.textBoxPhrase.Name = "textBoxPhrase";
            this.textBoxPhrase.Size = new System.Drawing.Size(350, 20);
            this.textBoxPhrase.TabIndex = 11;
            // 
            // textBoxStatus
            // 
            this.textBoxStatus.Location = new System.Drawing.Point(4, 61);
            this.textBoxStatus.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.textBoxStatus.Name = "textBoxStatus";
            this.textBoxStatus.ReadOnly = true;
            this.textBoxStatus.Size = new System.Drawing.Size(54, 20);
            this.textBoxStatus.TabIndex = 12;
            this.textBoxStatus.Text = "Idle";
            // 
            // buttonCancel
            // 
            this.buttonCancel.Enabled = false;
            this.buttonCancel.Location = new System.Drawing.Point(389, 12);
            this.buttonCancel.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(68, 25);
            this.buttonCancel.TabIndex = 13;
            this.buttonCancel.Text = "Stop";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonSpeak
            // 
            this.buttonSpeak.Location = new System.Drawing.Point(289, 12);
            this.buttonSpeak.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.buttonSpeak.Name = "buttonSpeak";
            this.buttonSpeak.Size = new System.Drawing.Size(46, 25);
            this.buttonSpeak.TabIndex = 14;
            this.buttonSpeak.Text = "TTS";
            this.buttonSpeak.UseVisualStyleBackColor = true;
            this.buttonSpeak.Click += new System.EventHandler(this.buttonSpeak_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.numericUpDownMinScore);
            this.groupBox1.Controls.Add(this.checkBoxShowAudioProblems);
            this.groupBox1.Controls.Add(this.checkBoxShowHypothesis);
            this.groupBox1.Controls.Add(this.textBoxStatus);
            this.groupBox1.Controls.Add(this.audioLevelBar);
            this.groupBox1.Location = new System.Drawing.Point(9, 10);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBox1.Size = new System.Drawing.Size(142, 110);
            this.groupBox1.TabIndex = 15;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Recognition";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(63, 85);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(58, 13);
            this.label1.TabIndex = 14;
            this.label1.Text = "Min. Score";
            // 
            // numericUpDownMinScore
            // 
            this.numericUpDownMinScore.Location = new System.Drawing.Point(6, 84);
            this.numericUpDownMinScore.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.numericUpDownMinScore.Maximum = new decimal(new int[] {
            95,
            0,
            0,
            0});
            this.numericUpDownMinScore.Minimum = new decimal(new int[] {
            75,
            0,
            0,
            0});
            this.numericUpDownMinScore.Name = "numericUpDownMinScore";
            this.numericUpDownMinScore.ReadOnly = true;
            this.numericUpDownMinScore.Size = new System.Drawing.Size(52, 20);
            this.numericUpDownMinScore.TabIndex = 13;
            this.numericUpDownMinScore.Value = new decimal(new int[] {
            85,
            0,
            0,
            0});
            // 
            // checkBoxEditPhrase
            // 
            this.checkBoxEditPhrase.AutoSize = true;
            this.checkBoxEditPhrase.Location = new System.Drawing.Point(404, 155);
            this.checkBoxEditPhrase.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.checkBoxEditPhrase.Name = "checkBoxEditPhrase";
            this.checkBoxEditPhrase.Size = new System.Drawing.Size(44, 17);
            this.checkBoxEditPhrase.TabIndex = 16;
            this.checkBoxEditPhrase.Text = "Edit";
            this.checkBoxEditPhrase.UseVisualStyleBackColor = true;
            this.checkBoxEditPhrase.CheckedChanged += new System.EventHandler(this.checkBoxEditPhrase_CheckedChanged);
            // 
            // timerEvent
            // 
            this.timerEvent.Tick += new System.EventHandler(this.timerEvent_Tick);
            // 
            // labelIsSpeaking
            // 
            this.labelIsSpeaking.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.labelIsSpeaking.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.labelIsSpeaking.Location = new System.Drawing.Point(267, 15);
            this.labelIsSpeaking.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelIsSpeaking.Name = "labelIsSpeaking";
            this.labelIsSpeaking.Size = new System.Drawing.Size(16, 18);
            this.labelIsSpeaking.TabIndex = 17;
            // 
            // labelIsRecognizing
            // 
            this.labelIsRecognizing.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.labelIsRecognizing.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.labelIsRecognizing.Location = new System.Drawing.Point(267, 43);
            this.labelIsRecognizing.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelIsRecognizing.Name = "labelIsRecognizing";
            this.labelIsRecognizing.Size = new System.Drawing.Size(16, 18);
            this.labelIsRecognizing.TabIndex = 19;
            // 
            // buttonClear
            // 
            this.buttonClear.Location = new System.Drawing.Point(389, 40);
            this.buttonClear.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.buttonClear.Name = "buttonClear";
            this.buttonClear.Size = new System.Drawing.Size(68, 25);
            this.buttonClear.TabIndex = 20;
            this.buttonClear.Text = "Clear";
            this.buttonClear.UseVisualStyleBackColor = true;
            this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
            // 
            // textBoxRecTimer
            // 
            this.textBoxRecTimer.Location = new System.Drawing.Point(230, 43);
            this.textBoxRecTimer.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.textBoxRecTimer.Name = "textBoxRecTimer";
            this.textBoxRecTimer.ReadOnly = true;
            this.textBoxRecTimer.Size = new System.Drawing.Size(34, 20);
            this.textBoxRecTimer.TabIndex = 21;
            this.textBoxRecTimer.Text = "0";
            this.textBoxRecTimer.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // textBoxUI
            // 
            this.textBoxUI.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxUI.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.textBoxUI.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxUI.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBoxUI.Location = new System.Drawing.Point(9, 177);
            this.textBoxUI.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.textBoxUI.Multiline = true;
            this.textBoxUI.Name = "textBoxUI";
            this.textBoxUI.ReadOnly = true;
            this.textBoxUI.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBoxUI.Size = new System.Drawing.Size(448, 321);
            this.textBoxUI.TabIndex = 0;
            // 
            // labelIsServerOK
            // 
            this.labelIsServerOK.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.labelIsServerOK.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.labelIsServerOK.Location = new System.Drawing.Point(267, 71);
            this.labelIsServerOK.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelIsServerOK.Name = "labelIsServerOK";
            this.labelIsServerOK.Size = new System.Drawing.Size(16, 18);
            this.labelIsServerOK.TabIndex = 22;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(286, 75);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 13);
            this.label3.TabIndex = 23;
            this.label3.Text = "Server Status";
            // 
            // numericUpDownRecTime
            // 
            this.numericUpDownRecTime.Location = new System.Drawing.Point(172, 42);
            this.numericUpDownRecTime.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.numericUpDownRecTime.Maximum = new decimal(new int[] {
            25,
            0,
            0,
            0});
            this.numericUpDownRecTime.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericUpDownRecTime.Name = "numericUpDownRecTime";
            this.numericUpDownRecTime.ReadOnly = true;
            this.numericUpDownRecTime.Size = new System.Drawing.Size(52, 20);
            this.numericUpDownRecTime.TabIndex = 15;
            this.numericUpDownRecTime.Value = new decimal(new int[] {
            20,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(172, 24);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 13);
            this.label2.TabIndex = 24;
            this.label2.Text = "Rec. Time";
            // 
            // buttonWAV
            // 
            this.buttonWAV.Location = new System.Drawing.Point(340, 12);
            this.buttonWAV.Name = "buttonWAV";
            this.buttonWAV.Size = new System.Drawing.Size(46, 25);
            this.buttonWAV.TabIndex = 25;
            this.buttonWAV.Text = "WAV";
            this.buttonWAV.UseVisualStyleBackColor = true;
            this.buttonWAV.Click += new System.EventHandler(this.buttonWAV_Click);
            // 
            // textBoxWAV
            // 
            this.textBoxWAV.Enabled = false;
            this.textBoxWAV.Location = new System.Drawing.Point(48, 128);
            this.textBoxWAV.Name = "textBoxWAV";
            this.textBoxWAV.ReadOnly = true;
            this.textBoxWAV.Size = new System.Drawing.Size(350, 20);
            this.textBoxWAV.TabIndex = 26;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 131);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(32, 13);
            this.label4.TabIndex = 27;
            this.label4.Text = "WAV";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(10, 156);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(28, 13);
            this.label5.TabIndex = 28;
            this.label5.Text = "Text";
            // 
            // buttonOpenWAV
            // 
            this.buttonOpenWAV.Location = new System.Drawing.Point(404, 126);
            this.buttonOpenWAV.Name = "buttonOpenWAV";
            this.buttonOpenWAV.Size = new System.Drawing.Size(53, 23);
            this.buttonOpenWAV.TabIndex = 29;
            this.buttonOpenWAV.Text = "Open";
            this.buttonOpenWAV.UseVisualStyleBackColor = true;
            this.buttonOpenWAV.Click += new System.EventHandler(this.buttonOpenWAV_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(466, 503);
            this.Controls.Add(this.buttonOpenWAV);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBoxWAV);
            this.Controls.Add(this.buttonWAV);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.numericUpDownRecTime);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.labelIsServerOK);
            this.Controls.Add(this.textBoxUI);
            this.Controls.Add(this.textBoxRecTimer);
            this.Controls.Add(this.buttonClear);
            this.Controls.Add(this.labelIsRecognizing);
            this.Controls.Add(this.labelIsSpeaking);
            this.Controls.Add(this.checkBoxEditPhrase);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.buttonSpeak);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.textBoxPhrase);
            this.Controls.Add(this.buttonRecognize);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.MaximumSize = new System.Drawing.Size(482, 640);
            this.MinimumSize = new System.Drawing.Size(482, 266);
            this.Name = "Form1";
            this.Text = "Speech Manager 1.1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMinScore)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRecTime)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ProgressBar audioLevelBar;
        private System.Windows.Forms.Button buttonRecognize;
        private System.Windows.Forms.CheckBox checkBoxShowAudioProblems;
        private System.Windows.Forms.CheckBox checkBoxShowHypothesis;
        private System.Windows.Forms.TextBox textBoxPhrase;
        private System.Windows.Forms.TextBox textBoxStatus;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonSpeak;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox checkBoxEditPhrase;
        private System.Windows.Forms.Timer timerEvent;
        private System.Windows.Forms.Label labelIsSpeaking;
        private System.Windows.Forms.Label labelIsRecognizing;
        private System.Windows.Forms.Button buttonClear;
        private System.Windows.Forms.TextBox textBoxRecTimer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numericUpDownMinScore;
        private System.Windows.Forms.TextBox textBoxUI;
        private System.Windows.Forms.Label labelIsServerOK;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numericUpDownRecTime;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonWAV;
        private System.Windows.Forms.TextBox textBoxWAV;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button buttonOpenWAV;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
    }
}

