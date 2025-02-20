#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "H2M/Core/RefH2M.h"

#include "DX11.h"

#include <unordered_set>

	
class DX11PhysicalDevice : public H2M::RefCountedH2M
{
public:
	DX11PhysicalDevice();
	~DX11PhysicalDevice();

	static H2M::RefH2M<DX11PhysicalDevice> Select();

	friend class DX11Device;
};

// Represents a logical device
class DX11Device : public H2M::RefCountedH2M
{
public:
	DX11Device(const H2M::RefH2M<DX11PhysicalDevice>& physicalDevice);
	~DX11Device();

	const H2M::RefH2M<DX11PhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }

	ID3D11Device* GetDX11Device() const { return m_LogicalDevice; }

private:
	H2M::RefH2M<DX11PhysicalDevice> m_PhysicalDevice;

	ID3D11Device* m_LogicalDevice; // m_d3d_device

};
