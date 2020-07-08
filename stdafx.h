#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

#pragma once

#include string
#include vector
#include algorithm
#include sstream
#include iostream
#include fstream
#include random

#ifdef _WIN32
#define NOMINMAX
#include Windows.h
#else
# include systime.h
#endif

#include cpprestjson.h
#include cppresthttp_listener.h
#include cppresturi.h
#include cpprestasyncrt_utils.h
#include cpprestjson.h
#include cpprestfilestream.h
#include cpprestcontainerstream.h
#include cpprestproducerconsumerstream.h

#pragma warning ( push )
#pragma warning ( disable  4457 )
#pragma warning ( pop )
#include locale
#include ctime
#endif  STDAFX_H_INCLUDED