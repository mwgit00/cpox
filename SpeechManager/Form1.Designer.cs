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
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMinScore)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownRecTime)).BeginInit();
            this.SuspendLayout();
            // 
            // audioLevelBar
            // 
            this.audioLevelBar.Location = new System.Drawing.Point(83, 75);
            this.audioLevelBar.Name = "audioLevelBar";
            this.audioLevelBar.Size = new System.Drawing.Size(100, 22);
            this.audioLevelBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.audioLevelBar.TabIndex = 1;
            // 
            // buttonRecognize
            // 
            this.buttonRecognize.Location = new System.Drawing.Point(382, 49);
            this.buttonRecognize.Name = "buttonRecognize";
            this.buttonRecognize.Size = new System.Drawing.Size(90, 31);
            this.buttonRecognize.TabIndex = 5;
            this.buttonRecognize.Text = "Recognize";
            this.buttonRecognize.UseVisualStyleBackColor = true;
            this.buttonRecognize.Click += new System.EventHandler(this.buttonRecognize_Click);
            // 
            // checkBoxShowAudioProblems
            // 
            this.checkBoxShowAudioProblems.AutoSize = true;
            this.checkBoxShowAudioProblems.Location = new System.Drawing.Point(6, 21);
            this.checkBoxShowAudioProblems.Name = "checkBoxShowAudioProblems";
            this.checkBoxShowAudioProblems.Size = new System.Drawing.Size(167, 21);
            this.checkBoxShowAudioProblems.TabIndex = 6;
            this.checkBoxShowAudioProblems.Text = "Show Audio Problems";
            this.checkBoxShowAudioProblems.UseVisualStyleBackColor = true;
            this.checkBoxShowAudioProblems.CheckedChanged += new System.EventHandler(this.checkBoxShowAudioProblems_CheckedChanged);
            // 
            // checkBoxShowHypothesis
            // 
            this.checkBoxShowHypothesis.AutoSize = true;
            this.checkBoxShowHypothesis.Location = new System.Drawing.Point(6, 48);
            this.checkBoxShowHypothesis.Name = "checkBoxShowHypothesis";
            this.checkBoxShowHypothesis.Size = new System.Drawing.Size(138, 21);
            this.checkBoxShowHypothesis.TabIndex = 7;
            this.checkBoxShowHypothesis.Text = "Show Hypothesis";
            this.checkBoxShowHypothesis.UseVisualStyleBackColor = true;
            // 
            // textBoxPhrase
            // 
            this.textBoxPhrase.Enabled = false;
            this.textBoxPhrase.Location = new System.Drawing.Point(12, 153);
            this.textBoxPhrase.Name = "textBoxPhrase";
            this.textBoxPhrase.Size = new System.Drawing.Size(492, 22);
            this.textBoxPhrase.TabIndex = 11;
            // 
            // textBoxStatus
            // 
            this.textBoxStatus.Location = new System.Drawing.Point(6, 75);
            this.textBoxStatus.Name = "textBoxStatus";
            this.textBoxStatus.ReadOnly = true;
            this.textBoxStatus.Size = new System.Drawing.Size(71, 22);
            this.textBoxStatus.TabIndex = 12;
            this.textBoxStatus.Text = "Idle";
            // 
            // buttonCancel
            // 
            this.buttonCancel.Enabled = false;
            this.buttonCancel.Location = new System.Drawing.Point(478, 12);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(90, 31);
            this.buttonCancel.TabIndex = 13;
            this.buttonCancel.Text = "Stop";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonSpeak
            // 
            this.buttonSpeak.Location = new System.Drawing.Point(382, 12);
            this.buttonSpeak.Name = "buttonSpeak";
            this.buttonSpeak.Size = new System.Drawing.Size(90, 31);
            this.buttonSpeak.TabIndex = 14;
            this.buttonSpeak.Text = "Say Text";
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
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(189, 135);
            this.groupBox1.TabIndex = 15;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Recognition";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(84, 105);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(75, 17);
            this.label1.TabIndex = 14;
            this.label1.Text = "Min. Score";
            // 
            // numericUpDownMinScore
            // 
            this.numericUpDownMinScore.Location = new System.Drawing.Point(8, 103);
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
            this.numericUpDownMinScore.Size = new System.Drawing.Size(70, 22);
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
            this.checkBoxEditPhrase.Location = new System.Drawing.Point(514, 153);
            this.checkBoxEditPhrase.Name = "checkBoxEditPhrase";
            this.checkBoxEditPhrase.Size = new System.Drawing.Size(54, 21);
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
            this.labelIsSpeaking.Location = new System.Drawing.Point(356, 18);
            this.labelIsSpeaking.Name = "labelIsSpeaking";
            this.labelIsSpeaking.Size = new System.Drawing.Size(20, 22);
            this.labelIsSpeaking.TabIndex = 17;
            // 
            // labelIsRecognizing
            // 
            this.labelIsRecognizing.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.labelIsRecognizing.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.labelIsRecognizing.Location = new System.Drawing.Point(356, 53);
            this.labelIsRecognizing.Name = "labelIsRecognizing";
            this.labelIsRecognizing.Size = new System.Drawing.Size(20, 22);
            this.labelIsRecognizing.TabIndex = 19;
            // 
            // buttonClear
            // 
            this.buttonClear.Location = new System.Drawing.Point(478, 49);
            this.buttonClear.Name = "buttonClear";
            this.buttonClear.Size = new System.Drawing.Size(90, 31);
            this.buttonClear.TabIndex = 20;
            this.buttonClear.Text = "Clear";
            this.buttonClear.UseVisualStyleBackColor = true;
            this.buttonClear.Click += new System.EventHandler(this.buttonClear_Click);
            // 
            // textBoxRecTimer
            // 
            this.textBoxRecTimer.Location = new System.Drawing.Point(306, 53);
            this.textBoxRecTimer.Name = "textBoxRecTimer";
            this.textBoxRecTimer.ReadOnly = true;
            this.textBoxRecTimer.Size = new System.Drawing.Size(44, 22);
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
            this.textBoxUI.Location = new System.Drawing.Point(12, 181);
            this.textBoxUI.Multiline = true;
            this.textBoxUI.Name = "textBoxUI";
            this.textBoxUI.ReadOnly = true;
            this.textBoxUI.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBoxUI.Size = new System.Drawing.Size(556, 412);
            this.textBoxUI.TabIndex = 0;
            // 
            // labelIsServerOK
            // 
            this.labelIsServerOK.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.labelIsServerOK.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.labelIsServerOK.Location = new System.Drawing.Point(356, 87);
            this.labelIsServerOK.Name = "labelIsServerOK";
            this.labelIsServerOK.Size = new System.Drawing.Size(20, 22);
            this.labelIsServerOK.TabIndex = 22;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(382, 92);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(94, 17);
            this.label3.TabIndex = 23;
            this.label3.Text = "Server Status";
            // 
            // numericUpDownRecTime
            // 
            this.numericUpDownRecTime.Location = new System.Drawing.Point(230, 52);
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
            this.numericUpDownRecTime.Size = new System.Drawing.Size(70, 22);
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
            this.label2.Location = new System.Drawing.Point(230, 29);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 17);
            this.label2.TabIndex = 24;
            this.label2.Text = "Rec. Time";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(580, 600);
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
            this.MaximumSize = new System.Drawing.Size(598, 680);
            this.MinimumSize = new System.Drawing.Size(598, 320);
            this.Name = "Form1";
            this.Text = "Speech Manager 1.0";
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
    }
}

