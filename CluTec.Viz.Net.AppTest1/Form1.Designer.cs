namespace Clu.Viz.Net.AppTest1
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
            this.CluCtrl1 = new System.Windows.Forms.PictureBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.panelCluCtrl = new System.Windows.Forms.Panel();
            this.richTextCluOutput = new System.Windows.Forms.RichTextBox();
            this.numUDBrightness = new System.Windows.Forms.NumericUpDown();
            this.numUDContrast = new System.Windows.Forms.NumericUpDown();
            this.butSetImgB = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.CluCtrl1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panelCluCtrl.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numUDBrightness)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numUDContrast)).BeginInit();
            this.SuspendLayout();
            // 
            // CluCtrl1
            // 
            this.CluCtrl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CluCtrl1.Location = new System.Drawing.Point(0, 0);
            this.CluCtrl1.Name = "CluCtrl1";
            this.CluCtrl1.Size = new System.Drawing.Size(620, 820);
            this.CluCtrl1.TabIndex = 0;
            this.CluCtrl1.TabStop = false;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.CluCtrl1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.panelCluCtrl);
            this.splitContainer1.Size = new System.Drawing.Size(1007, 820);
            this.splitContainer1.SplitterDistance = 620;
            this.splitContainer1.TabIndex = 1;
            // 
            // panelCluCtrl
            // 
            this.panelCluCtrl.Controls.Add(this.butSetImgB);
            this.panelCluCtrl.Controls.Add(this.richTextCluOutput);
            this.panelCluCtrl.Controls.Add(this.numUDBrightness);
            this.panelCluCtrl.Controls.Add(this.numUDContrast);
            this.panelCluCtrl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelCluCtrl.Location = new System.Drawing.Point(0, 0);
            this.panelCluCtrl.Name = "panelCluCtrl";
            this.panelCluCtrl.Size = new System.Drawing.Size(383, 820);
            this.panelCluCtrl.TabIndex = 0;
            // 
            // richTextCluOutput
            // 
            this.richTextCluOutput.Location = new System.Drawing.Point(35, 218);
            this.richTextCluOutput.Name = "richTextCluOutput";
            this.richTextCluOutput.Size = new System.Drawing.Size(324, 327);
            this.richTextCluOutput.TabIndex = 2;
            this.richTextCluOutput.Text = "";
            // 
            // numUDBrightness
            // 
            this.numUDBrightness.Location = new System.Drawing.Point(37, 128);
            this.numUDBrightness.Name = "numUDBrightness";
            this.numUDBrightness.Size = new System.Drawing.Size(120, 26);
            this.numUDBrightness.TabIndex = 1;
            this.numUDBrightness.ValueChanged += new System.EventHandler(this.numUDBrightness_ValueChanged);
            // 
            // numUDContrast
            // 
            this.numUDContrast.Location = new System.Drawing.Point(37, 48);
            this.numUDContrast.Name = "numUDContrast";
            this.numUDContrast.Size = new System.Drawing.Size(120, 26);
            this.numUDContrast.TabIndex = 0;
            this.numUDContrast.ValueChanged += new System.EventHandler(this.numUDContrast_ValueChanged);
            // 
            // butSetImgB
            // 
            this.butSetImgB.Location = new System.Drawing.Point(87, 603);
            this.butSetImgB.Name = "butSetImgB";
            this.butSetImgB.Size = new System.Drawing.Size(102, 48);
            this.butSetImgB.TabIndex = 3;
            this.butSetImgB.Text = "Image B";
            this.butSetImgB.UseVisualStyleBackColor = true;
            this.butSetImgB.Click += new System.EventHandler(this.butSetImgB_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1007, 820);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.CluCtrl1)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panelCluCtrl.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numUDBrightness)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numUDContrast)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox CluCtrl1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Panel panelCluCtrl;
        private System.Windows.Forms.NumericUpDown numUDBrightness;
        private System.Windows.Forms.NumericUpDown numUDContrast;
        private System.Windows.Forms.RichTextBox richTextCluOutput;
        private System.Windows.Forms.Button butSetImgB;
    }
}

