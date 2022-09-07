using Libdl

# const path_to_lib = pwd()
#addHeaderDir(path_to_lib, kind=C_System) # <2>
#cxxinclude("mycpplib.h") # <3>
path_to_lib = raw"I:\code\wfdb_wrapper\build\Release"

lib = Libdl.dlopen(joinpath(path_to_lib, "cphysiofile.dll"), Libdl.RTLD_GLOBAL) # <4>
lib_name = :cphysiofile

a = ccall((:get2, lib_name), Int32, ())
a = ccall((:f, lib_name), Nothing, ())
b = ccall((:sumMyArgs, lib_name), Float64, (Float32,Float32), 2.5, 1.5)
b = ccall((:showWchars, lib_name), Nothing, (Cwstring,), "Привет!")
Cwstring("куку")

struct CSomeData
    x::Cint
    y::Cdouble
    #z::Cstring
end
b = ccall((:getSomeData, lib_name), CSomeData, (Int32, Float64), 1, 2.0)

struct CSomeData1
    x::Int32
    y::Float64
    #z::Cstring
end
b = ccall((:getSomeData, lib_name), CSomeData1, (Int32, Float64), 1, 2.0)

struct CSomeData2
    x::Int32
    y::Float64
    #z::String
end
b = ccall((:getSomeData, lib_name), CSomeData2, (Int32, Float64), 1, 2.0)

struct CSomeData3
    x::Int64
    y::Float64
    #z::String
end
b = ccall((:getSomeData, lib_name), CSomeData3, (Int32, Float64), 1, 2.0)

struct CSomeData4
    x::Int32
    y::Float32
    #z::String
end
b = ccall((:getSomeData, lib_name), CSomeData4, (Int32, Float64), 1, 2.0)

b = ccall((:getSomeData, lib_name), Tuple{Int32, Float64}, (Int32, Float64), 1, 2.0)
b

b = ccall((:SomeData, lib_name), CSomeData, (Int32, Float64), 1, 2.0)

