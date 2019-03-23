////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluViz.Plugin.StdLib.rtl
// file:      FuncDef.h
//
// summary:   Declares the function definition class
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

#pragma once

#include "CluTec.Viz.Parse/CodeFunction.h"

#include "Func_Scene.h"
#include "Func_Texture.h"
#include "Func_Capture.h"
#include "Func_Material.h"
#include "Func_Peek.h"
#include "Func_Blend.h"
#include "Func_ClipPlane.h"
#include "Func_RenderTarget.h"

#include "Func_Draw.h"
#include "Func_DrawConfig.h"
#include "Func_Light.h"
#include "Func_Fog.h"
#include "Func_Color.h"
#include "Func_VisConfig.h"
#include "Func_Frame.h"
#include "Func_Shader.h"
#include "Func_GLTool.h"

#include "Func_IO.h"
#include "Func_Process.h"
#include "Func_Visualize.h"

#include "Func_File.h"
#include "Func_Mouse.h"
#include "Func_Window.h"
#include "Func_Tool.h"
#include "Func_Environ.h"

#include "Func_String.h"
#include "Func_Debug.h"
#include "Func_Info.h"
#include "Func_List.h"
#include "Func_Text.h"
#include "Func_Latex.h"
#include "Func_HTML.h"
#include "Func_UnitConversion.h"

#include "Func_Math.h"
#include "Func_Random.h"
#include "Func_GA.h"
#include "Func_Matrix.h"
#include "Func_Tensor.h"
#include "Func_ErrorProp.h"

#include "Func_C2_ErrorProp.h"
#include "Func_C2_Define.h"
#include "Func_C2_Algo.h"

#include "Func_E3_Define.h"

#include "Func_E8_Define.h"

#include "Func_N3_Define.h"
#include "Func_N3_ErrorProp.h"

#include "Func_P3_Define.h"

#include "Func_Object_Basic.h"
#include "Func_Object_Form.h"


SMapExtFunc pFuncList[] =
{
	/////////////////////////////////////////////////////////////////////////
	/// Gl Tool Functions

	{ "Tool", ToolFunc },


	/////////////////////////////////////////////////////////////////////////
	/// Shader Functions

	{ "Shader", ShaderFunc },

	{ "ShaderSupported", ShaderSupportedFunc },
	{ "Scene:Shader:Supported", ShaderSupportedFunc },

	{ "EnableShader", EnableShaderFunc },
	{ "Scene:Shader:Enable", EnableShaderFunc },

	{ "EnableShaderForPicking", EnableShaderForPickingFunc },
	{ "Scene:Shader:EnablePicking", EnableShaderForPickingFunc },

	{ "ShaderBuild", ShaderBuildFunc },
	{ "Scene:Shader:Build", ShaderBuildFunc },

	{ "SetShaderUniformVar", SetShaderUniformVarFunc },
	{ "Scene:Shader:Var", SetShaderUniformVarFunc },

	{ "AnimShader", AnimShaderFunc },
	{ "Scene:Shader:Anim", AnimShaderFunc },

	////////////////////////////////////////////////////////////
	/// Draw Functions

	{ "Plot", PlotFunc },

	{ "FlushDraw", FlushDrawFunc },

	{ "DrawFrame", DrawFrameFunc },
	{ "DrawPlane", DrawPlaneFunc },
	{ "DrawPolygon", DrawPolygonFunc },
	{ "DrawBox", DrawBoxFunc },
	{ "DrawLine", DrawLineFunc },
	{ "DrawPoint", DrawPointFunc },
	{ "DrawPointSurface", DrawPointSurfaceFunc },
	{ "DrawPointGrid", DrawPointGridFunc },
	{ "DrawPointList", DrawPointListFunc },

	{ "DrawArrow", DrawArrowFunc },
	{ "DrawArc", DrawArcFunc },
	{ "DrawDisk", DrawDiskFunc },
	{ "DrawEllipse", DrawEllipseFunc },
	{ "DrawEllipsoid", DrawEllipsoidFunc },
	{ "DrawCircle", DrawCircleFunc },
	{ "DrawCircleSurface", DrawCircleSurfaceFunc },
	{ "DrawEllipseSurface", DrawEllipseSurfaceFunc },
	{ "DrawLineSurface", DrawLineSurfaceFunc },
	{ "DrawTriangle", DrawTriangleFunc },
	{ "DrawCone", DrawConeFunc },
	{ "DrawSphere", DrawSphereFunc },
	{ "DrawCylinder", DrawCylinderFunc },
	{ "DrawIcosahedron", DrawIcosahedronFunc },
	{ "DrawQuantumFractal", DrawQuantumFractalFunc },
	{ "DrawQuantumFractalSurface", DrawQuantumFractalSurfaceFunc },

	////////////////////////////////////////////////////////////
	/// Draw Configuration Functions

	{ "SetSphereDetailLevel", SetSphereDetailLevelFunc },
	{ "SetCylinderDetailLevel", SetCylinderDetailLevelFunc },
	{ "SetArrowShape", SetArrowShapeFunc },

	{ "SetMode", SetModeFunc },
	{ "SetPlotMode", SetPlotModeFunc },
	{ "SetPointSize", SetPointSizeFunc },
	{ "SetLineWidth", SetLineWidthFunc },

	////////////////////////////////////////////////////////////
	/// Visualization Configuration

	{ "EnableAntiAliasing", EnableMultisampleFunc },

	{ "EnableColorStereo", EnableColorStereoFunc },
	{ "SetColorStereoMask", SetColorStereoMaskFunc },
	{ "SetColorStereoPars", SetColorStereoParsFunc },

	{ "EnableTransparency", EnableTransparencyFunc },

	{ "EnablePointSprites", EnablePointSpritesFunc },

	{ "SetVisPrec", SetVisPrecFunc },

	////////////////////////////////////////////////////////////
	/// Camera Functions

	{ "SetRTViewMode", SetRTViewModeFunc },
	{ "ResetView", ResetViewFunc },
	{ "SetRTViewLookAt", SetRTViewLookAtFunc },
	{ "SetRTPosition", SetRTPositionFunc },
	{ "SetRTViewRotationAxisInversion", SetRTViewRotationAxisInversionFunc },

	////////////////////////////////////////////////////////////
	/// Lighting Functions

	{ "EnableLighting", EnableLightingFunc },

	//{ "ResetLight",  ResetLightFunc },
	{ "AmbientLight", AmbientLightFunc },
	{ "SetAmbientLight", SetAmbientLightFunc },

	{ "Light", LightFunc },
	{ "EnableLight", EnableLightFunc },
	{ "Scene:Light:Enable", EnableLightFunc },

	//{ "EnableLightFixed",  EnableLightFixedFunc },
	{ "SetLightPos", SetLightPosFunc },
	{ "Scene:Light:Pos", SetLightPosFunc },

	{ "SetLightAmbient", SetLightAmbientColFunc },
	{ "Scene:Light:Ambient", SetLightAmbientColFunc },

	{ "SetLightDiffuse", SetLightDiffuseColFunc },
	{ "Scene:Light:Diffuse", SetLightDiffuseColFunc },

	{ "SetLightSpecular", SetLightSpecularColFunc },
	{ "Scene:Light:Specular", SetLightSpecularColFunc },

	{ "SetLightSpotDir", SetLightSpotDirFunc },
	{ "Scene:Light:SpotDir", SetLightSpotDirFunc },

	{ "SetLightSpotExp", SetLightSpotExpFunc },
	{ "Scene:Light:SpotExp", SetLightSpotExpFunc },

	{ "SetLightSpotCutOff", SetLightSpotCutOffFunc },
	{ "Scene:Light:SpotCutOff", SetLightSpotCutOffFunc },

	{ "SetLightAttenuation", SetLightAttenuationFunc },
	{ "Scene:Light:Attenuation", SetLightAttenuationFunc },

	////////////////////////////////////////////////////////////
	/// Color Functions

	{ "Color", ColorFunc },
	{ "SetColor", SetColorFunc },
	{ "AnimColor", AnimColorFunc },

	////////////////////////////////////////////////////////////
	/// Frame Functions

	{ "AnimRotateFrame", AnimRotateFrameFunc },
	{ "AnimTranslateFrame", AnimTranslateFrameFunc },
	{ "AnimScaleFrame", AnimScaleFrameFunc },

	{ "PushFrame", PushFrameFunc },
	{ "PopFrame", PopFrameFunc },

	{ "Frame", FrameFunc },
	{ "ResetFrame", ResetFrameFunc },
	{ "SetFrame", SetFrameFunc },
	{ "MultiplyFrame", MultiplyFrameFunc },
	{ "TranslateFrame", TranslateFrameFunc },
	{ "RotateFrame", RotateFrameFunc },
	{ "ReflectFrame", ReflectFrameFunc },
	{ "ScaleFrame", ScaleFrameFunc },
	{ "AutoBillboardFrame", AutoBillboardFrameFunc },
	{ "AutoTextureFrame", AutoTextureFrameFunc },
	{ "AutoPixelSizeFrame", AutoPixelSizeFrameFunc },

	////////////////////////////////////////////////////////////
	/// Scene Functions
	{ "GetSceneGraph", GetSceneGraphFunc },

	{ "StartOverlay", StartOverlayFunc },
	{ "EndOverlay", EndOverlayFunc },
	{ "StartView", StartViewFunc },
	{ "EndView", EndViewFunc },

	{ "Scene", SceneFunc },
	{ "CopyScene", CopySceneFunc },
	{ "DrawToScene", DrawToSceneFunc },
	{ "ClearScene", ClearSceneFunc },
	{ "SetPickScene", SetPickSceneFunc },
	{ "ClearPickScene", ClearPickSceneFunc },
	{ "SetScenePickTrigger", SetScenePickTriggerFunc },
	{ "DoSceneAdaptiveRedraw", DoSceneAdaptiveRedrawFunc },
	{ "EnableSceneAdaptiveRedraw", EnableSceneAdaptiveRedrawFunc },
	{ "EnableSceneAutoAdaptFrontFace", EnableSceneAutoAdaptFrontFaceFunc },
	{ "EnableScenePick", EnableScenePickFunc },
	{ "EnableScenePickTarget", EnableScenePickTargetFunc },
	{ "EnableSceneNotify", EnableSceneNotifyFunc },
	{ "EnableSceneDrawModes", EnableSceneDrawModesFunc },
	{ "EnableSceneAutoAdjustFrame", EnableSceneAutoAdjustFrameFunc },
	{ "EnableSceneLocalTransform", EnableSceneLocalTransformFunc },
	{ "EnableSceneAutoTranslate", EnableSceneAutoTranslateFunc },
	{ "EnableSceneAutoRotate", EnableSceneAutoRotateFunc },
	{ "EnableSceneAutoScale", EnableSceneAutoScaleFunc },
	{ "EnableSceneAutoScaleImage", EnableSceneAutoPixelZoomFunc },
	{ "EnableSceneAnimDrag", EnableSceneAnimDragFunc },
	{ "EnableSceneDrag", EnableSceneDragFunc },
	{ "EnableSceneDragInScreenPlane", EnableSceneDragInScreenPlaneFunc },
	{ "EnableSceneDragInProjBasis", EnableSceneDragInProjBasisFunc },
	{ "EnableSceneDragRange", EnableSceneDragRangeFunc },
	{ "EnableSceneResetFrame", EnableSceneResetFrameFunc },
	{ "EnableScene", EnableSceneFunc },
	{ "EnableSceneNormalize", EnableSceneNormalizeFunc },
	{ "GetSceneDragData", GetSceneDragDataFunc },
	{ "SetSceneDragData", SetSceneDragDataFunc },
	{ "SetSceneAnimDragAttenuation", SetSceneAnimDragAttenuationFunc },
	{ "SetSceneAutoDragControl", SetSceneAutoDragControlFunc },
	{ "GetSceneDragFactor", GetSceneDragFactorFunc },
	{ "SetSceneDragFactor", SetSceneDragFactorFunc },
	{ "SetSceneDragRange", SetSceneDragRangeFunc },
	{ "SetSceneViewAngle", SetSceneViewAngleFunc },
	{ "SetSceneViewVolume", SetSceneViewVolumeFunc },
	{ "SetSceneOverlay", SetSceneOrthoProjFunc },
	{ "SetScenePixelOverlay", SetScenePixelOrthoProjFunc },

	{ "GetSceneViewportSize", GetSceneViewportSizeFunc },
	{ "SetSceneViewport", SetSceneViewportFunc },
	{ "EnableSceneViewportScissor", EnableSceneViewportScissorFunc },
	{ "SetSceneViewportOriginRef", SetSceneViewportOriginRefFunc },
	{ "SetSceneViewportLocal", SetSceneViewportLocalFunc },
	{ "SetSceneViewportAlign", SetSceneViewportAlignFunc },
	{ "SetSceneViewportBorder", SetSceneViewportBorderFunc },
	{ "SetSceneRepeatCount", SetSceneRepeatCountFunc },
	{ "SetSceneAutoAdjustFrame", SetSceneAutoAdjustFrameFunc },
	{ "SetSceneFrameContext", SetSceneFrameContextFunc },
	{ "SetSceneDragBasis", SetSceneDragBasisFunc },
	{ "SetSceneScaleFunc", SetSceneScaleFuncFunc },
	{ "SetSceneScaleType", SetSceneScaleTypeFunc },

	{ "SetSceneDepthLevel", SetSceneDepthLevelFunc },
	{ "SetSceneDepthLevelType", SetSceneDepthLevelTypeFunc },
	{ "SetSceneDepthLevelLocalRange", SetSceneDepthLevelLocalRangeFunc },

	{ "EnableSceneLocalTime", EnableSceneLocalTimeFunc },
	{ "SetSceneLocalTime", SetSceneLocalTimeFunc },
	{ "SetSceneLocalTimeRef", SetSceneLocalTimeRefFunc },

	/////////////////////////////////////////////////////////////////////////
	/// Texture Functions

	{ "Texture", TextureFunc },
	{ "SetTextureImage", SetTextureImageFunc },
	{ "SetTexture3d", SetTexture3dFunc },
	{ "SetTextureWrapType", SetTextureWrapTypeFunc },
	{ "SetTextureBlendType", SetTextureBlendTypeFunc },
	{ "SetTextureUnit", SetTextureUnitFunc },
	{ "GetTextureType", GetTextureTypeFunc },
	{ "EnableTextureInterpolate", EnableTextureInterpolateFunc },
	{ "EnableTextureForPicking", EnableTextureForPickingFunc },

	////////////////////////////////////////////////////////////
	/// Capture Functions

	{ "Capture", CaptureFunc },
	{ "EnableCapture", EnableCaptureFunc },
	{ "SetCaptureTarget", SetCaptureTargetFunc },
	{ "SetCaptureArea", SetCaptureAreaFunc },
	{ "SetCaptureTargetOrigin", SetCaptureTargetOriginFunc },

	////////////////////////////////////////////////////////////
	/// Material Functions

	{ "Material", MaterialFunc },
	{ "SetMaterialEmission", SetMaterialEmissionFunc },
	{ "SetMaterialSpecular", SetMaterialSpecularFunc },
	{ "SetMaterialShininess", SetMaterialShininessFunc },

	////////////////////////////////////////////////////////////
	/// Peek Functions

	{ "Peek", PeekFunc },
	{ "GetPeekViewExtentLocal", GetPeekViewExtentLocalFunc },
	{ "GetPeekViewExtentScreen", GetPeekViewExtentScreenFunc },

	////////////////////////////////////////////////////////////
	/// Blend Functions
	{ "Blend", BlendFunc },

	////////////////////////////////////////////////////////////
	/// Clipping Plane Functions
	{ "ClipPlane", ClipPlaneFunc },
	{ "SetClipPlane", SetClipPlaneFunc },
	{ "EnableClipPlane", EnableClipPlaneFunc },

	////////////////////////////////////////////////////////////
	/// Render Target functions
	{ "RenderTarget", RenderTargetFunc },
	{ "EnableRenderTarget", EnableRenderTargetFunc },
	{ "SetRenderTargetSize", SetRenderTargetSizeFunc },
	{ "GetRenderTargetImage", GetRenderTargetImageFunc },
	{ "EnableRenderTargetSnapshot", EnableRenderTargetSnapshotFunc },

	////////////////////////////////////////////////////////////
	/// Process Functions

	{ "Image", NewBMPFunc },

	{ "Img2Matrix", BMP2MatrixFunc },
	{ "Image2Matrix", BMP2MatrixFunc },

	{ "Matrix2Img", Matrix2BMPFunc },
	{ "Matrix2Image", Matrix2BMPFunc },

	{ "FlipImg", FlipBMPFunc },
	{ "FlipImage", FlipBMPFunc },

	{ "Rotate90Img", Rotate90BMPFunc },
	{ "Rotate90Image", Rotate90BMPFunc },

	//{ "RotateImg", RotateBMPFunc },
	{ "CopyImg", CopyBMPFunc },
	{ "CopyImage", CopyBMPFunc },

	{ "ResizeImg", ResizeBMPFunc },
	{ "ResizeImage", ResizeBMPFunc },

	{ "ReshapeImg", ReshapeBMPFunc },
	{ "ReshapeImage", ReshapeBMPFunc },

	{ "InsertImg", InsertBMPFunc },
	{ "InsertImage", InsertBMPFunc },

	{ "GetImgBoundingBox", GetBMPBoundingBoxFunc },
	{ "GetImageBoundingBox", GetBMPBoundingBoxFunc },

	{ "ClearImg", ClearBMPFunc },
	{ "ClearImage", ClearBMPFunc },

	{ "ConvertImgType", ConvertBMPTypeFunc },
	{ "ConvertImageType", ConvertBMPTypeFunc },

	{ "SampleImgArea", SampleImgAreaFunc },

	////////////////////////////////////////////////////////////
	/// Info Functions

	{ "GetImgType", GetBMPTypeFunc },
	{ "GetImageType", GetBMPTypeFunc },

	////////////////////////////////////////////////////////////
	// I/O Functions

	{ "ReadImg", LoadBMPFunc },
	{ "ReadImage", LoadBMPFunc },

	{ "WriteImg", SaveBMPFunc },
	{ "WriteImage", SaveBMPFunc },

	////////////////////////////////////////////////////////////
	/// Visualize Functions

	{ "SetImgScale", SetBMPScaleFunc },
	{ "SetImageScale", SetBMPScaleFunc },

	{ "SetImgAlign", SetBMPAlignFunc },
	{ "SetImageAlign", SetBMPAlignFunc },

	{ "SetImgPos", SetBMPPosFunc },
	{ "SetImagePos", SetBMPPosFunc },

	{ "SetTexture", SetBMPTextureFunc },

	////////////////////////////////////////////////////////////
	/// File Functions

	{ "GetDirList", GetDirListFunc },

	{ "WriteVar", WriteVariableFunc },
	{ "ReadVar", ReadVariableFunc },
	{ "ReadXML", ReadXMLFunc },
	{ "WriteXML", WriteXMLFunc },
	{ "ReadText", ReadTextFunc },
	{ "WriteText", WriteTextFunc },
	{ "ReadData", ReadDataFunc },
	{ "WriteData", WriteDataFunc },
	{ "ReadMatrix", ReadMatrixFunc },
	{ "ShowFile", ShowFileFunc },
	{ "FileChooser", FileChooserFunc },
	{ "SaveScreen", SaveScreenFunc },

	////////////////////////////////////////////////////////////
	/// Trix Functions

	//{ "OpenTrix", OpenTrixFunc },
	//{ "GetTrixFrameCount", GetTrixFrameCountFunc },
	//{ "GetTrixFrame", GetTrixFrameFunc },
	//{ "GetTrixFrameDepthRange", GetTrixFrameDepthRangeFunc },

	////////////////////////////////////////////////////////////
	/// Mouse Functions

	{ "SetMouseMode", SetMouseModeFunc },
	{ "GetMouseMode", GetMouseModeFunc },

	{ "SetMouseValue", SetMouseValueFunc },
	{ "GetMouseValue", GetMouseValueFunc },

	{ "SetMouseSens", SetMouseSensFunc },
	{ "GetMouseSens", GetMouseSensFunc },

	{ "Mouse", MouseFunc },

	////////////////////////////////////////////////////////////
	/// Window related functions

	{ "EnableMenu", EnableMenuFunc },
	{ "EnableStdCtrl", EnableStdCtrlFunc },
	{ "SetMenu", SetMenuFunc },

	{ "SetInfoText", SetInfoTextFunc },
	{ "SetInfoWidth", SetInfoWidthFunc },

	{ "GetVisWinSize", GetOGLSizeFunc },
	{ "SetVisWinSize", SetOGLSizeFunc },

	////////////////////////////////////////////////////////////
	/// Tool Functions

	{ "Slider", ToolSliderFunc },
	//{ "Stepper", ToolCounterFunc },
	{ "Input", ToolInputFunc },
	{ "InputText", ToolInputTextFunc },
	{ "CheckBox", ToolCheckBoxFunc },
	{ "Button", ToolButtonFunc },
	//{ "ImageButton", ToolImageButtonFunc },
	{ "Choice", ToolChoiceFunc },

	{ "ToolbarImageButton", ToolbarImageButtonFunc },
	{ "ToolbarStepper", ToolbarCounterFunc },
	{ "ToolbarLabel", ToolbarLabelFunc },

	////////////////////////////////////////////////////////////
	/// Time

	{ "GetTime", GetTimeFunc },

	{ "EnableAnimate", EnableAnimationFunc },
	{ "SetAnimateTimeStep", SetAnimationTimeStepFunc },
	{ "SetVisualizationTimeStep", SetVisualizationTimeStepFunc },

	////////////////////////////////////////////////////////////
	/// Event

	{ "EnableEvent", EnableEventFunc },

	////////////////////////////////////////////////////////////
	/// Presentation Functions

	{ "_ClearScriptList", ClearScriptListFunc },
	{ "_AddScriptToList", AddScriptToListFunc },

	////////////////////////////////////////////////////////////
	/// Text Functions

	{ "GetTextImg", GetTextImageFunc },
	{ "GetTextImage", GetTextImageFunc },

	{ "SetTextFont", SetTextFontFunc },
	{ "SetTextMagStep", SetTextMagStepFunc },
	{ "SetTextBorder", SetTextBorderFunc },

	////////////////////////////////////////////////////////////
	/// Render Text Functions

	{ "GetRenderTextImage", GetRenderTextImageFunc },

	{ "SetRenderTextFont", SetRenderTextFontFunc },
	{ "SetRenderTextSize", SetRenderTextSizeFunc },
	{ "SetRenderTextStyle", SetRenderTextStyleFunc },
	{ "SetRenderTextWeight", SetRenderTextWeightFunc },
	{ "SetRenderTextStretch", SetRenderTextStretchFunc },
	{ "SetRenderTextAlignment", SetRenderTextAlignmentFunc },
	{ "SetRenderTextWordWrapWidth", SetRenderTextWordWrapWidthFunc },
	{ "SetRenderTextAlpha", SetRenderTextAlphaFunc },

	////////////////////////////////////////////////////////////
	/// Latex Functions

	{ "Latex", LatexStringFunc },
	{ "DrawLatex", DrawLatexFunc },

	{ "GetLatexImg", GetLatexImageFunc },
	{ "GetLatexImage", GetLatexImageFunc },

	{ "SetLatexMagStep", SetLatexMagStepFunc },
	{ "SetLatexMainFont", SetLatexMainFontFunc },
	{ "EnableRenderLatex", SetRenderLatexFunc },

	////////////////////////////////////////////////////////////
	/// HTML Functions

	{ "HTML", HTMLFunc },

	///////////////////////////////////////////////////////
	/// List Functions

	{ "argtrue", ArgTrueFunc },
	{ "sort", SortFunc },

	{ "List", ListFunc },
	{ "SubList", ExtractListElementsFunc },
	{ "RemList", RemoveListElementsFunc },
	{ "InsList", InsertListElementsFunc },
	{ "TransList", TransposeListElementsFunc },

	{ "CombIdxList", CombinationIndexListFunc },
	{ "PermIdxList", PermutationIndexListFunc },

	///////////////////////////////////////////////////////
	/// String Functions

	{ "SetTextPrec", SetTextPrecFunc },
	{ "String", StringFunc },
	{ "String2ASCII", String2ASCIIFunc },
	{ "ASCII2String", ASCII2StringFunc },
	{ "Scalar", ScalarFunc },
	{ "Counter", CounterFunc },
	{ "HexStr2Scalar", HexStr2ScalarFunc },
	{ "Scalar2HexStr", Scalar2HexStrFunc },

	///////////////////////////////////////////////////////
	/// Info Functions

	{ "Size", SizeFunc },
	{ "Type", TypeFunc },

	///////////////////////////////////////////////////////
	/// Debug Functions

	{ "_GetBaseElementRepositoryContentList", GetBaseElementRepositoryContentListFunc },
	{ "_GetImageRepositoryContentList", GetImageRepositoryContentListFunc },

	///////////////////////////////////////////////////////
	/// Unit Conversion functions
	// TODO: Implement Unit conversion
	//{ "ConvertUnitFromMM", ConvertUnitFromMMFunc },

	////////////////////////////////////////////////////////////
	/// C2

	{ "DefVarsC2", DefVarsEl2 },
	{ "VecC2", VecEl2Func },
	{ "DegConic", MakeConicDegenerateFunc },
	{ "FindAllDegConics", FindAllDegConicsFunc },

	{ "RotorC2", RotorC2Func },
	{ "ReflectorC2", ReflectorC2Func },

	/// C2 Error Propagation
	{ "EPVecC2", ErrorPropVecC2Func },

	////////////////////////////////////////////////////////////
	/// E3

	{ "VecE3", VecE3Func },
	{ "RotorE3", RotorE3Func },
	{ "DefVarsE3", DefVarsE3 },

	////////////////////////////////////////////////////////////
	/// E8

#ifdef USE_GA_E8
	{ "VecE8", VecE8Func },
	{ "RotorE8", RotorE8Func },
	{ "DefVarsE8", DefVarsE8 },
#endif

	////////////////////////////////////////////////////////////
	/// N3

	{ "DefVarsN3", DefVarsN3 },

	{ "VecN3", VecN3Func },
	{ "RotorN3", RotorN3Func },

	{ "SphereN3", SphereN3 },
	{ "TranslatorN3", TranslatorN3 },
	{ "DilatorN3", DilatorN3 },

	{ "DirVecN3", DirVecN3 },

	/// N3 Error Propagation
	{ "EPVecN3", ErrorPropVecN3Func },
	{ "EPCCVecN3", ErrorPropCrossCovVecN3Func },
	{ "EPPrjVecN3ToE3", ErrorPropPrjVecN3ToE3Func },

	////////////////////////////////////////////////////////////
	/// P3

	{ "DefVarsP3", DefVarsP3 },
	{ "VecP3", VecP3Func },
	{ "RotorP3", RotorP3Func },
	{ "DirVecP3", DirVecP3Func },
	{ "AffineVecP3", AffineVecP3Func },

	////////////////////////////////////////////////////////////
	/// General Functions

	{ "SetEvalPrec", SetEvalPrecFunc },

	////////////////////////////////////////////////////////////
	/// Mathematical functions

	{ "sum", SumFunc },
	{ "sub", SubFunc },
	{ "prod", ProdFunc },
	//{ "arg", ArgFunc },
	{ "min", MinFunc },
	{ "max", MaxFunc },
	{ "argmin", ArgMinFunc },
	{ "argmax", ArgMaxFunc },

	{ "abs", AbsFunc },
	{ "pow", PowFunc },
	{ "sqrt", SqrtFunc },
	{ "inv", InvFunc },
	{ "fact", FactFunc },
	{ "sign", SignFunc },
	{ "sin", SinFunc },
	{ "cos", CosFunc },
	{ "tan", TanFunc },
	{ "asin", AsinFunc },
	{ "acos", AcosFunc },
	{ "atan", AtanFunc },
	{ "sinh", SinHypFunc },
	{ "cosh", CosHypFunc },
	{ "tanh", TanHypFunc },

	{ "floor", FloorFunc },
	{ "ceil", CeilFunc },
	{ "round", RoundFunc },
	{ "exp", ExpFunc },
	{ "log", LogFunc },

	{ "isNaN", IsNaNFunc },
	{ "NaN", NaNFunc },

	////////////////////////////////////////////////////////////
	/// Geometric algebra functions
	{ "oprod", OuterProdFunc },
	{ "iprod", InnerProdFunc },

	{ "proj", ProjectFunc },
	{ "rej", RejectFunc },
	{ "cp", CPFunc },
	{ "acp", ACPFunc },

	{ "FactorizeBlade", FactorBladeFunc },

	{ "GradeList", GradeListFunc },
	{ "BladeIdxList", GetMVIndicesFunc },
	{ "MV2Mask", GenerateMaskFunc },
	{ "AnalyzeMV", AnalyzeMVFunc },

	////////////////////////////////////////////////////////////
	/// Random Number Generator Functions

	{ "Ran", RandomFunc },
	{ "SetRanSeed", RandomSeedFunc },
	{ "Gauss", GaussFunc },
	{ "SetGaussPars", GaussParametersFunc },

	////////////////////////////////////////////////////////////
	/// Matrix Functions
	{ "Matrix", MatrixFunc },
	{ "ResizeMatrix", ResizeMatrixFunc },
	{ "ReshapeMatrix", ReshapeMatrixFunc },
	{ "AddRows", AddMatrixRowsFunc },
	{ "AddCols", AddMatrixColsFunc },
	{ "RowSpace", ReduceMatrixToRowSpaceFunc },
	{ "FindMainNRows", ReduceMatrixToMajorRowsFunc },
	{ "GetRows", ExtractMatrixRowsFunc },

	{ "EigenValues", EigenValuesFunc },
	{ "Eigen", EigenFunc },
	{ "SVD", SVDFunc },
	{ "det", DetFunc },

	{ "Diag2Row", DiagToVectorFunc },
	{ "Row2Diag", VectorToDiagFunc },
	{ "DiagToRow", DiagToVectorFunc },
	{ "RowToDiag", VectorToDiagFunc },
	{ "IdMatrix", IdentityMatrixFunc },

	{ "GetMVProductMatrix", GetMVProductMatrixFunc },
	{ "MV2Matrix", GetMVMatrixFunc },
	{ "Matrix2MV", GetMatrixMVFunc },

	{ "Sym2Col", Sym2ColMatrixFunc },
	{ "Col2Sym", Col2SymMatrixFunc },

	////////////////////////////////////////////////////////////
	/// Tensor Functions
	{ "Tensor", TensorFunc },
	{ "MV2Tensor", GetMVTensorFunc },
	{ "Tensor2MV", GetTensorMVFunc },
	{ "GAOpTensor", GetGAOpTensorFunc },

	////////////////////////////////////////////////////////////
	/// Error Propagation

	{ "EPOp", ErrorPropagationOperationFunc },

	{ "EPsqrt", ErrorPropSQRTFunc },
	{ "EPinv", ErrorPropINVFunc },
	{ "EPmul", ErrorPropMULFunc },


	////////////////////////////////////////////////////////////
	/// Object Basic Functions

	{ "Object", VexListFunc },

	{ "CopyObject", CopyVexListFunc },
	{ "CopyFromObject", CopyFromVexListFunc },

	{ "Scene:Object:Copy", CopyVexListFunc },

	{ "ClearObject", ClearVexListFunc },
	{ "Scene:Object:Clear", ClearVexListFunc },

	{ "EnableObjectKeepData", EnableVexListKeepDataFunc },
	{ "Scene:Object:KeepData", EnableVexListKeepDataFunc },

	{ "SetObjectMode", SetVexListTypeFunc },
	{ "Scene:Object:Mode", SetVexListTypeFunc },

	{ "SetObjectStructure", SetVexListPolyModeFunc },
	{ "Scene:Object:Structure", SetVexListPolyModeFunc },

	{ "SetObjectScale", SetVexListScaleFunc },
	{ "Scene:Object:Scale", SetVexListScaleFunc },

	{ "SetObjectLineStipple", SetVexListLineStippleFunc },
	{ "Scene:Object:LineStipple", SetVexListLineStippleFunc },

	{ "SetObjectInstanceCount", SetVexListInstanceCountFunc },
	{ "Scene:Object:InstanceCount", SetVexListInstanceCountFunc },

	{ "GetObjectData", GetVexListDataFunc },
	{ "Scene:Object:Data", GetVexListDataFunc },

	////////////////////////////////////////////////////////////
	/// Object Form Functions

	{ "SetObjectForm", SetVexListFormFunc },
	{ "Scene:Object:Form", SetVexListFormFunc },

	{ "AdaptObjectForm", AdaptVexListFormFunc },
	{ "Scene:Object:AdaptForm", AdaptVexListFormFunc },

	////////////////////////////////////////////////////////////////////////
	// End of list
	{ 0, 0 }
};

