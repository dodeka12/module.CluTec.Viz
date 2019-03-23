////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Net.AppTest2
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

namespace CluTec.Viz.Net.AppTest2
{
    public partial class Form1 : Form
    {
        private Clu.Viz.Net.Wnd.Engine m_xEngine;
        private Clu.Viz.Net.Wnd.View m_xView;

        public Form1()
        {
            InitializeComponent();

            m_xEngine = new Clu.Viz.Net.Wnd.Engine();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            m_xEngine.Start();
            m_xEngine.ScriptPath = Clu.Viz.Net.Env.GetPath(EnvPaths.Script_Root);

            m_xView = new Clu.Viz.Net.Wnd.View();
            

            m_xView.Create(10, 10, 300, 300, "Test 2", true, true, true, false);
             
            string sScriptFile = "CluTec.Viz.Net.View.ImageTool\\Main.clu";
            m_xView.LoadScript(sScriptFile);

            string sImageFile = m_xEngine.ScriptPath + "\\StdGuiLib\\ImgEx1.jpg";
            Clu.Net.CluImage xImage = Clu.Viz.Net.Env.LoadImage(sImageFile);

            m_xView.SetVarImage("imgX", xImage);
            m_xView.ExecScript("SetImage");
        }
    }
}
