////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.AppTest1
// file:      Form1.cs
//
// summary:   Implements the form 1 class
//
//            Copyright (c) 2019 by Christian Perwass.
//
//            This file is part of the CluTecLib library.
//
//            The CluTecLib library is free software: you can redistribute it and / or modify
//            it under the terms of the GNU Lesser General Public License as published by
//            the Free Software Foundation, either version 3 of the License, or
//            (at your option) any later version.
//
//            The CluTecLib library is distributed in the hope that it will be useful,
//            but WITHOUT ANY WARRANTY; without even the implied warranty of
//            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//            GNU Lesser General Public License for more details.
//
//            You should have received a copy of the GNU Lesser General Public License
//            along with the CluTecLib library.
//            If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Clu.Viz.Net;

namespace Clu.Viz.Net.AppTest1
{

    public partial class Form1 : Form
    {
        View.Engine m_xEngine;
        View.Control m_xControl;
        View.ScriptVar m_varContrast;
        View.ScriptVar m_varBrightness;
        View.ScriptVar m_varImgA;
        View.ScriptVar m_varImgB;
        View.ScriptVar m_varImgX;
        View.ScriptMessageArgs m_xMsgArgs;

        View.Mesh3dTool m_xMesh3d;

        event View.ScriptMessageHandler ScriptMessage;

        public Form1()
        {
            InitializeComponent();

            m_xEngine = View.Engine.GetEngine();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            m_xEngine.Start();
            m_xControl = new View.Control(CluCtrl1);

            m_xMesh3d = new View.Mesh3dTool();
            m_xMesh3d.AttachTo(m_xControl);


            //m_xMsgArgs = new View.ScriptMessageArgs();

            //ScriptMessage += m_xControl.Script.OnScriptMessage;

            //string sScriptPath = Env.GetPath(EnvPaths.Script_Root);
            //sScriptPath += @"\CluTec.Viz.Net.AppTest1\Main.clu";

            //m_xControl.Script.StoreOutput = true;
            //m_xControl.Script.Filename = sScriptPath;

            //m_varContrast = m_xControl.Script.GetVar("dContrast");
            //m_varBrightness = m_xControl.Script.GetVar("dBrightness");
            //m_varImgA = m_xControl.Script.GetVar("imgA");
            //m_varImgB = m_xControl.Script.GetVar("imgB");

            //m_varImgX = m_xControl.Script.GetVar("imgX");
            //m_varImgX.SendMessageToScriptOnChange = true;
            //m_varImgX.ScriptMessageName = "SetImage";

            //m_varContrast.SendMessageToScriptOnChange = true;
            //m_varContrast.ScriptMessageName = "Contrast";

            //m_varBrightness.SendMessageToScriptOnChange = true;
            //m_varBrightness.ScriptMessageName = "Brightness";

            //numUDContrast.Value = m_varContrast.GetDecimal();
            //numUDBrightness.Value = m_varBrightness.GetDecimal();




            //m_xMsgArgs.Message = "Init";
            //m_xMsgArgs.DoReadOutput = true;
            //ScriptMessage(this, m_xMsgArgs);

            //richTextCluOutput.Text = m_xMsgArgs.Output;
        }


        private void numUDBrightness_ValueChanged(object sender, EventArgs e)
        {
            //m_varBrightness.Set(numUDBrightness.Value);
            //richTextCluOutput.Text = m_varBrightness.Script.Output;
        }

        private void numUDContrast_ValueChanged(object sender, EventArgs e)
        {
            //m_varContrast.Set(numUDContrast.Value);
            //richTextCluOutput.Text = m_varContrast.Script.Output;
        }

        private void butSetImgB_Click(object sender, EventArgs e)
        {
            //string sImageFile = Env.GetPath(EnvPaths.Script_Root) + "\\CluTec.Viz.Net.AppTest1\\ImgEx2.jpg";
            //Clu.Net.CluImage xImg = Env.LoadImage(sImageFile);
            //m_varImgX.Set(xImg);
            //richTextCluOutput.Text = m_varImgX.Script.Output;

            //sImageFile = Env.GetPath(EnvPaths.Script_Root) + "\\CluTec.Viz.Net.AppTest1\\ImgEx2_.png";
            //Env.SaveImage(sImageFile, xImg);
        }
    }
}
