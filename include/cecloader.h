/*
 * This file is part of the libCEC(R) library.
 *
 * libCEC(R) is Copyright (C) 2011 Pulse-Eight Limited.  All rights reserved.
 * libCEC(R) is an original work, containing original code.
 *
 * libCEC(R) is a trademark of Pulse-Eight Limited.
 *
 * This program is dual-licensed; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * Alternatively, you can license this library under a commercial license,
 * please contact Pulse-Eight Licensing for more information.
 *
 * For more information contact:
 * Pulse-Eight Licensing       <license@pulse-eight.com>
 *     http://www.pulse-eight.com/
 *     http://www.pulse-eight.net/
 */

#ifndef CECLOADER_H_
#define CECLOADER_H_

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <conio.h>

HINSTANCE g_libCEC = NULL;

CEC::ICECAdapter *LoadLibCec(const char *strName, CEC::cec_logical_address iLogicalAddress = CEC::CECDEVICE_PLAYBACKDEVICE1, uint16_t iPhysicalAddress = CEC_DEFAULT_PHYSICAL_ADDRESS, const char *strLib = NULL)
{
  if (!g_libCEC)
    g_libCEC = LoadLibrary(strLib ? strLib : "libcec.dll");
  if (!g_libCEC)
    return NULL;

  typedef void* (__cdecl*_CreateLibCec)(const char *, uint8_t, uint16_t);
  _CreateLibCec CreateLibCec;
  CreateLibCec = (_CreateLibCec) (GetProcAddress(g_libCEC, "CECCreate"));
  if (!CreateLibCec)
    return NULL;
  return static_cast< CEC::ICECAdapter* > (CreateLibCec(strName, (uint8_t) iLogicalAddress, iPhysicalAddress));
}

void UnloadLibCec(CEC::ICECAdapter *device)
{
  typedef void (__cdecl*_DestroyLibCec)(void * device);
  _DestroyLibCec DestroyLibCec;
  DestroyLibCec = (_DestroyLibCec) (GetProcAddress(g_libCEC, "CECDestroy"));
  if (DestroyLibCec)
    DestroyLibCec(device);

  FreeLibrary(g_libCEC);
  g_libCEC = NULL;
}

#else

#include <dlfcn.h>

void *g_libCEC = NULL;

CEC::ICECAdapter *LoadLibCec(const char *strName, CEC::cec_logical_address iLogicalAddress = CEC::CECDEVICE_PLAYBACKDEVICE1, uint16_t iPhysicalAddress = CEC_DEFAULT_PHYSICAL_ADDRESS, const char *strLib = NULL)
{
  if (!g_libCEC)
  {
#if defined(__APPLE__)
    g_libCEC = dlopen(strLib ? strLib : "libcec.dylib", RTLD_LAZY);
#else
    g_libCEC = dlopen(strLib ? strLib : "libcec.so", RTLD_LAZY);
#endif
    if (!g_libCEC)
    {
#if defined(__APPLE__)
      cout << "cannot find " << (strLib ? strLib : "libcec.dylib") << endl;
#else
      cout << "cannot find " << (strLib ? strLib : "libcec.so") << endl;
#endif
      return NULL;
    }
  }

  typedef void* _CreateLibCec(const char *, uint8_t, uint16_t);
  _CreateLibCec* CreateLibCec = (_CreateLibCec*) dlsym(g_libCEC, "CECCreate");
  if (!CreateLibCec)
  {
    cout << "cannot find CreateLibCec" << endl;
    return NULL;
  }

  return (CEC::ICECAdapter*) CreateLibCec(strName, iLogicalAddress, iPhysicalAddress);
}

void UnloadLibCec(CEC::ICECAdapter *device)
{
  typedef void* _DestroyLibCec(CEC::ICECAdapter *);
  _DestroyLibCec *DestroyLibCec = (_DestroyLibCec*) dlsym(g_libCEC, "CECDestroy");
  if (DestroyLibCec)
    DestroyLibCec(device);

  dlclose(g_libCEC);
}

#endif

#endif /* CECLOADER_H_ */
