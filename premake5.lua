-- premake5.lua
workspace "EgyptsRayTracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "EgyptsRayTracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "EgyptsRayTracer"