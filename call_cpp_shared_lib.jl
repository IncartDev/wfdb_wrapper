using Cxx
using Libdl

# const path_to_lib = pwd()
#addHeaderDir(path_to_lib, kind=C_System) # <2>
#cxxinclude("mycpplib.h") # <3>
path_to_lib = raw"I:\code\wfdb_wrapper\build\Release"

lib = Libdl.dlopen(joinpath(path_to_lib, "libshared.dll"), Libdl.RTLD_GLOBAL) # <4>

ccall((:f, joinpath(path_to_lib, "libshared.dll")),  Nothing, ())

myclib = raw"I:\code\wfdb_wrapper\build\Release\shared.dll"
#myclib = raw"I:\code\wfdb_wrapper\build\Debug\shared.dll"

a = ccall((:get2, myclib), Int32, ())
a = ccall((:f, myclib), Nothing, ())
b = ccall((:sumMyArgs, myclib), Float64, (Float32,Float32), 2.5, 1.5)

struct CSomeData
    x::Cint
    y::Cdouble
    #z::Cstring
end
b = ccall((:getSomeData, myclib), CSomeData, (Int32, Float64), 1, 2.0)

struct CSomeData1
    x::Int32
    y::Float64
    #z::Cstring
end
b = ccall((:getSomeData, myclib), CSomeData1, (Int32, Float64), 1, 2.0)

struct CSomeData2
    x::Int32
    y::Float64
    #z::String
end
b = ccall((:getSomeData, myclib), CSomeData2, (Int32, Float64), 1, 2.0)

struct CSomeData3
    x::Int64
    y::Float64
    #z::String
end
b = ccall((:getSomeData, myclib), CSomeData3, (Int32, Float64), 1, 2.0)

struct CSomeData4
    x::Int32
    y::Float32
    #z::String
end
b = ccall((:getSomeData, myclib), CSomeData4, (Int32, Float64), 1, 2.0)

b = ccall((:getSomeData, myclib), Tuple{Int32, Float64}, (Int32, Float64), 1, 2.0)
b

b = ccall((:SomeData, myclib), CSomeData, (Int32, Float64), 1, 2.0)

