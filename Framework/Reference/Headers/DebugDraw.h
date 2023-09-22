//--------------------------------------------------------------------------------------
// File: DebugDraw.h
//
// Helpers for drawing various debug shapes using PrimitiveBatch
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once

#include "Engine_Defines.h"

namespace DX
{
    void ENGINE_DLL XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingSphere& sphere,
        FXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingBox& box,
        FXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingOrientedBox& obb,
        FXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
        const BoundingFrustum& frustum,
        FXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV DrawGrid(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR xAxis, FXMVECTOR yAxis,
        FXMVECTOR origin, size_t xdivs, size_t ydivs,
        GXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR origin, FXMVECTOR majorAxis, FXMVECTOR minorAxis,
        GXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV DrawRay(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR origin, FXMVECTOR direction, bool normalize = true,
        FXMVECTOR color = Colors::White);

    void ENGINE_DLL XM_CALLCONV DrawTriangle(PrimitiveBatch<VertexPositionColor>* batch,
        FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR pointC,
        GXMVECTOR color = Colors::White);
}