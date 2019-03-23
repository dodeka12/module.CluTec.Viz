////////////////////////////////////////////////////////////////////////////////////////////////////
// project:   CluTec.Viz.Draw
// file:      OGLBaseElement.h
//
// summary:   Declares the ogl base element class
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

// Class written by Vladimir Banarer, 3.12.2001.

// OGLBaseElement.h: Schnittstelle f�r die Klasse COGLBaseElement.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLBASEELEMENT_H__D6A14A39_EB0B_4DB8_B0AB_19F0A9EA7FD9__INCLUDED_)
	#define AFX_OGLBASEELEMENT_H__D6A14A39_EB0B_4DB8_B0AB_19F0A9EA7FD9__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif

#include <string>
#include <map>
#include <list>
#include <vector>

#include "OGLBEReference.h"
//#include "CluTec.Viz.ImgRepo/CvCoreImgRepo.h"
#include "MatrixStack.h"

	using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	The base class of all OpenGL elements.
/// </summary>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CLUDRAW_API COGLBaseElement
	{
	public:

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Values that represent apply modes.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		enum EApplyMode
		{
			DRAW = 0,
			PICK = 1
		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	An apply data.
		/// </summary>
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		struct SApplyData
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// 	Default constructor.
			/// </summary>
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			SApplyData()
			{
				bAnimate            = false;
				bNeedAnimate        = false;
				bDispListRecord     = false;
				bUseColorStereo     = false;
				bDoDrag             = false;
				bAutoAdaptFrontFace = false;
				bInvertFrontFace    = false;
				bInTransparencyPass = false;

				// Is only true if rendering in first pass
				bInFirstRenderPass = false;
				// Is only true if rendering in last pass
				bInLastRenderPass = false;

				fColorStereoSep      = 0.0f;
				fColorStereoDegAngle = 0.0f;

				dTime     = 0.0;
				dTimeStep = 0.0;
				dPickTime = 0.0;

				iMouseX      = iMouseY = 0;
				iPickW       = iPickH = 0;
				iMousePrevX  = iMousePrevY = 0;
				iMouseStartX = iMouseStartY = 0;

				iRepeatIdx = 0;

				iStackDepth[0] = 0;
				iStackDepth[1] = 0;
				iStackDepth[2] = 0;

				for (int i = 0; i < OGL_MAX_TEX_UNITS; ++i)
				{
					pbActTexUnit[i] = false;
				}

				bMultiTexture = false;
				bInPickState  = false;

				pCurShader    = nullptr;
				pfCurColor[0] = 1.0f;
				pfCurColor[1] = 1.0f;
				pfCurColor[2] = 1.0f;
				pfCurColor[3] = 1.0f;

				pCurRenderTarget = nullptr;
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// 	Query if this object is multi textured.
			/// </summary>
			///
			/// <returns> True if multi textured, false if not. </returns>
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			bool IsMultiTextured()
			{
				int iTextures = 0;
				for (int i = 0; i < OGL_MAX_TEX_UNITS; ++i)
				{
					iTextures += (pbActTexUnit[i]);
				}

				return iTextures > 1;
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>
			/// 	Query if 'uUID' is picked.
			/// </summary>
			///
			/// <param name="uUID"> The UID. </param>
			///
			/// <returns> True if picked, false if not. </returns>
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			bool IsPicked(uint uUID) const
			{
				for (const auto& uPickUID : vecCurPickUID)
				{
					if (uUID == uPickUID)
					{
						return true;
					}
				}

				return false;
			}

			bool IsPicked(uint uUID, uint& uDrawn) const
			{
				for (uint i = 0; i < vecCurPickUID.size(); i++)
				{
					if (uUID == vecCurPickUID[i])
					{
						uDrawn = vecCurPickDrawFlag[i];
						return true;
					}
				}

				return false;
			}

			bool IsPicked(const std::map<uint, uint>& mapPickUID, uint& uDrawn) const
			{
				for (uint i = 0; i < vecCurPickUID.size(); i++)
				{
					if (mapPickUID.find(vecCurPickUID[i]) != mapPickUID.end())
					{
						uDrawn = vecCurPickDrawFlag[i];
						return true;
					}
				}

				return false;
			}

			void FlagPickDraw(uint uUID)
			{
				for (uint i = 0; i < vecCurPickUID.size(); i++)
				{
					if (uUID == vecCurPickUID[i])
					{
						vecCurPickDrawFlag[i] = 1;
					}
				}
			}

			void FlagPickDraw(const std::map<uint, uint>& mapPickUID)
			{
				for (uint i = 0; i < vecCurPickUID.size(); i++)
				{
					if (mapPickUID.find(vecCurPickUID[i]) != mapPickUID.end())
					{
						vecCurPickDrawFlag[i] = 1;
						break;
					}
				}
			}

			void ResetPickDrawFlags()
			{
				std::fill(vecCurPickDrawFlag.begin(), vecCurPickDrawFlag.end(), 0);
			}

			void SwapPickTime()
			{
				double dVal = dTime;

				dTime     = dPickTime;
				dPickTime = dVal;
			}

			int PushModelview() { return ++iStackDepth[0]; }
			int PushProjection() { return ++iStackDepth[1]; }
			int PushTexture() { return ++iStackDepth[2]; }

			int PopModelview() { if (iStackDepth[0]) { return --iStackDepth[0]; } return 0; }
			int PopProjection() { if (iStackDepth[1]) { return --iStackDepth[1]; } return 0;  }
			int PopTexture() { if (iStackDepth[2]) { return --iStackDepth[2]; } return 0;  }

			bool IsStackEmptyModelview() { return iStackDepth[0] == 0;  }
			bool IsStackEmptyProjection() { return iStackDepth[1] == 0;  }
			bool IsStackEmptyTexture() { return iStackDepth[2] == 0;  }

			// Initialize pick name data
			void InitPickNames()
			{
				// No pick names in stack
				uNextPickNameStackIdx = 0;

				// No pick name stacks in list.
				uNextPickNameStackListIdx = 0;

				// Set Pick name stack length to max length
				vecPickNameStack.resize(cuMaxPickNameStackLen);

				// Set pick name stack list to max length.
				// First element of each stored name stack is the stack's length
				vecPickNameStackList.resize(cuMaxPickNameStackListLen * (cuMaxPickNameStackLen + 1));

				vecCurPickSelUID.reserve(cuMaxPickNameStackLen);
				vecCurPickUID.reserve(cuMaxPickNameStackLen);
				vecCurPickDrawFlag.reserve(cuMaxPickNameStackLen);
			}

			// Push a name onto the pick name stack
			void PushPickName(uint uName)
			{
				_ASSERTE(uNextPickNameStackIdx < cuMaxPickNameStackLen);

				if (uNextPickNameStackIdx < cuMaxPickNameStackLen)
				{
					vecPickNameStack[uNextPickNameStackIdx] = uName;
				}
				// increment uNextPickNameStackIdx in any case to allow
				// for same number of Pops as Pushes even if not all names
				// can be stored in stack.
				++uNextPickNameStackIdx;
			}

			// Pop a name from the pick name stack
			void PopPickName()
			{
				if (uNextPickNameStackIdx > 0)
				{
					--uNextPickNameStackIdx;
				}
			}

			// Push a pick name stack into the pick name stack list
			// and return the index in the list where the name stack is stored.
			uint StorePickNameStack()
			{
				_ASSERTE(uNextPickNameStackIdx < cuMaxPickNameStackLen);
				_ASSERTE(uNextPickNameStackListIdx < cuMaxPickNameStackListLen);

				if (uNextPickNameStackListIdx < cuMaxPickNameStackListLen)
				{
					uint uPos = uNextPickNameStackListIdx * (cuMaxPickNameStackLen + 1);
					vecPickNameStackList[uPos] = uNextPickNameStackIdx;

					memcpy(&vecPickNameStackList[uPos + 1], vecPickNameStack.data(), cuMaxPickNameStackLen * sizeof(uint));
				}

				// increment index counter in any case to see how many elements we couldn't store.
				++uNextPickNameStackListIdx;
				return uNextPickNameStackListIdx - 1;
			}

			// Load the pick name stack stored at the given index
			// into vecPickNameStack und also set uNextPickNameStackIdx accordingly.
			// Functions returns false if the given index is invalid.
			bool LoadPickNameStack(uint uIdx)
			{
				if (uIdx >= cuMaxPickNameStackListLen)
				{
					return false;
				}

				uint uPos = uIdx * (cuMaxPickNameStackLen + 1);
				uNextPickNameStackIdx = vecPickNameStackList[uPos];
				memcpy(vecPickNameStack.data(), &vecPickNameStackList[uPos + 1], cuMaxPickNameStackLen * sizeof(uint));
				return true;
			}

			// Convert a given object name to a RGBA color
			static uint ConvertNameToColor(uint uBaseName, uint uPartName)
			{
				unsigned uValue = 0x00000FFF - (0x00000FFF & uBaseName);
				uValue = (uValue << 20) | (0x000FFFFF & uPartName);
				return uValue;
			}

			// Convert a given RGBA color to an object name
			static bool ConvertColorToName(uint& uBaseName, uint& uPartName, uint uColor)
			{
				uBaseName = (uColor & 0xFFF00000) >> 20;
				uBaseName = 0x00000FFF - uBaseName;
				uPartName = (uColor & 0x000FFFFF);

				return uBaseName != 0x00000FFF;
			}

			// Maximum length of pick name stack
			static const uint cuMaxPickNameStackLen = 32;

			// Maximum length of pick name stack list
			static const uint cuMaxPickNameStackListLen = 4096;

			// The idx in vecPickNameStack, where the next name is written to.
			//
			uint uNextPickNameStackIdx;

			// The current name stack for picking.
			// Max length is give by cuMaxPickNameStackLen;
			std::vector<uint> vecPickNameStack;

			// The idx in the list vecPickNameStackList where the next PickNameStack
			// is stored at an atomic drawing event. This variable therefore
			// also gives the number of pick name stacks in the list.
			uint uNextPickNameStackListIdx;

			// List of pick name stack instances stored at drawing events.
			// This is a list of N times cuMaxPickNameStackLen entries, where
			// N is the number of unique drawing events. A drawing events is
			// the drawing of an atomic object (e.g. vertex list) which is drawn
			// in with the same ID color in picking mode. Only atomic objects
			// can be picked. From the ID color of the atomic object the corresponding
			// name stack can be recovered from this list.
			std::vector<uint> vecPickNameStackList;

			std::vector<uint> vecCurPickSelUID;	// List of scene UIDs that are currently selected, e.g. through mouse drag
			std::vector<uint> vecCurPickUID;	// List of scene UIDs that are currently picked
			std::vector<uint> vecCurPickDrawFlag;	// Flag whether pick scene of picked obj. has already been drawn
			uint uCurPickPartID;
			uint uCurPickPartSelID;

			// Set to true during execution of scene graph if within a scene that is picked.
			bool bInPickState;

			bool bAnimate;
			bool bNeedAnimate;
			bool bDispListRecord;	// True if currently display list is recorded
			bool bUseColorStereo;	// True if color stereo is active
			bool bDoDrag;
			bool bInTransparencyPass;	// Drawing is in transparency pass
			bool bInFirstRenderPass;// Drawing is in first rendering pass
			bool bInLastRenderPass;	// Drawing is in last rendering pass

			// If true, the front face of vertex lists is adapted
			// according to the current model view frame.
			// If frame is reflected (determinant < 0), then the
			// front face is inverted.
			bool bAutoAdaptFrontFace;
			bool bInvertFrontFace;

			bool bShiftDown;
			bool bCtrlDown;
			bool bAltDown;

			float fColorStereoSep;
			float fColorStereoDegAngle;

			double dTime;
			double dTimeStep;
			double dPickTime;
			double pdGlobalProjMat[16];

			int iMouseX, iMouseY;
			int iMousePrevX, iMousePrevY;
			int iMouseStartX, iMouseStartY;

			// Width and Height in pixel for picking area used in function gluPickMatrix().
			int iPickW, iPickH;

			// Current total repeat index.
			int iRepeatIdx;

			// Depth level data
			// The number of slices the total depth range [0,1] is split into to draw viewports at different depth levels.
			int iDepthLevelCount;
			// The total width of a depth level including the gap to the next one
			double dDepthLevelWidth;
			// The gap between consecutive depth levels.
			double dDepthLevelGap;
			// The near value of the central depth level
			double dDepthLevelOrig;
			// The current depth level index. Need not be an integer value.
			double dDepthLevelIdx;

			int iStackDepth[3];	// modelview, projection, texture

			bool bMultiTexture;
			bool pbActTexUnit[OGL_MAX_TEX_UNITS];

			// The currently active shader if the reference is valid.
			COGLBaseElement* pCurShader;

			// The current render target
			COGLBaseElement* pCurRenderTarget;

			// The currently set color
			float pfCurColor[4];

			// The list of ClipPlanes
			Mem<float> mClipPlanes;

			/// <summary> Stack of matrices. </summary>
			Clu::CMatrixStack xMatrixStack;
		};

	public:

		COGLBaseElement();
		COGLBaseElement(const string sName);
		COGLBaseElement(const COGLBaseElement& rBaseElement);
		virtual ~COGLBaseElement();

		COGLBaseElement& operator=(const COGLBaseElement& rBaseElement)
		{
			m_sName      = rBaseElement.m_sName;
			m_listParent = rBaseElement.m_listParent;
			//m_uUID = rBaseElement.m_uUID;
			return *this;
		}

		virtual COGLBaseElement* Copy() = 0;

		// iMode allows different types of Apply calls.
		// Arbitrary data may be passed by pvData.
		virtual bool Apply(EApplyMode eMode, COGLBaseElement::SApplyData& rData) = 0;

		void SetName(const string sName) { m_sName = sName;  }
		string GetName() const { return m_sName; }

		string GetTypeName() const { return m_sTypeName; }

		uint GetUID() const { return m_uUID; }

		bool AddParent(COGLBaseElement* pParent);
		bool RemoveParent(COGLBaseElement* pParent);

	protected:

		string  m_sName;	// Human readable name (not necessarily unique)
		string  m_sTypeName;	// Human readable type name

		// List of parents
		list<COGLBaseElement*> m_listParent;

	private:

		uint    m_uUID;			// Unique ID for picking
		//static uint sm_uLastUID;	// Last used unique ID
	};

#endif	// !defined(AFX_OGLBASEELEMENT_H__D6A14A39_EB0B_4DB8_B0AB_19F0A9EA7FD9__INCLUDED_)
