#ifndef CIMGTOOLS_H
#define CIMGTOOLS_H

//#define use_openmp

#ifdef use_openmp
#include <omp.h>
#define cimg_use_openmp
#endif

#define cimg_display 0
#include "CImg.h"

using namespace cimg_library;

template<typename T,typename F>
bool save_metaimage(const CImgList<T>& img,const char *const headerFilename, const F *const scale=0, const F *const translation=0, const F *const affine=0, F offsetT=0, F scaleT=0, int isPerspective=0)
{
    if(!img.size()) return false;

    std::ofstream fileStream (headerFilename, std::ofstream::out);

    if (!fileStream.is_open())	{	std::cout << "Can not open " << headerFilename << std::endl;	return false; }

    fileStream << "ObjectType = Image" << std::endl;

    unsigned int dim[]={(unsigned)img(0).width(),(unsigned)img(0).height(),(unsigned)img(0).depth(), img.size()};
    unsigned int nbdims=(dim[3]==1)?3:4; //  for 2-d, we still need z scale dimension

    fileStream << "NDims = " << nbdims << std::endl;

    fileStream << "ElementNumberOfChannels = " << img(0).spectrum() << std::endl;

    fileStream << "DimSize = "; for(unsigned int i=0;i<nbdims;i++) fileStream << dim[i] << " "; fileStream << std::endl;

    fileStream << "ElementType = ";
    if(!strcmp(cimg::type<T>::string(),"char")) fileStream << "MET_CHAR" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"double")) fileStream << "MET_DOUBLE" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"float")) fileStream << "MET_FLOAT" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"int")) fileStream << "MET_INT" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"long")) fileStream << "MET_LONG" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"short")) fileStream << "MET_SHORT" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"unsigned char")) fileStream << "MET_UCHAR" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"unsigned int")) fileStream << "MET_UINT" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"unsigned long")) fileStream << "MET_ULONG" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"unsigned short")) fileStream << "MET_USHORT" << std::endl;
    else if(!strcmp(cimg::type<T>::string(),"bool")) fileStream << "MET_BOOL" << std::endl;
    else fileStream << "MET_UNKNOWN" << std::endl;

    if(scale) { fileStream << "ElementSpacing = "; for(unsigned int i=0;i<3;i++) if(i<nbdims) fileStream << scale[i] << " "; if(nbdims==4) fileStream << scaleT; fileStream << std::endl; }

    if(translation) { fileStream << "Position = "; for(unsigned int i=0;i<3;i++) if(i<nbdims) fileStream << translation[i] << " "; if(nbdims==4) fileStream << offsetT; fileStream << std::endl; }

    if(affine) { fileStream << "Orientation = "; for(unsigned int i=0;i<9;i++) fileStream << affine[i] << " "; fileStream << std::endl; }

    fileStream << "isPerpective = " << isPerspective << std::endl;

    std::string imageFilename(headerFilename);
    imageFilename.replace(imageFilename.find_last_of('.')+1,imageFilename.size(),"raw");

    // write filename without path
    std::string str(imageFilename);
    std::size_t pos=str.find_last_of('/');    if(pos==std::string::npos) pos=str.find_last_of('\\');    if(pos!=std::string::npos) str.erase(0, pos + 1);
    fileStream << "ElementDataFile = " << str.c_str() << std::endl;
    fileStream.close();

    std::FILE *const nfile = std::fopen(imageFilename.c_str(),"wb");
    if(!nfile) return false;

    cimglist_for(img,l)     cimg::fwrite(img(l)._data,img(l).size(),nfile);
    cimg::fclose(nfile);
    return true;
}



template<typename T,typename F>
CImgList<T> load_metaimage(const char *const  headerFilename, F *const scale=0, F *const translation=0, F *const affine=0, F *const offsetT=0, F *const scaleT=0, int *const isPerspective=0)
{
    CImgList<T> ret;

    std::ifstream fileStream(headerFilename, std::ifstream::in);
    if (!fileStream.is_open())	{	std::cout << "Can not open " << headerFilename << std::endl;	return ret; }

    std::string str,str2,imageFilename;
    unsigned int nbchannels=1,nbdims=4,dim[] = {1,1,1,1}; // 3 spatial dimas + time
    std::string inputType(cimg::type<T>::string());
    while(!fileStream.eof())
    {
        fileStream >> str;

        if(!str.compare("ObjectType"))
        {
            fileStream >> str2; // '='
            fileStream >> str2;
            if(str2.compare("Image")) { std::cout << "MetaImageReader: not an image ObjectType "<<std::endl; return ret;}
        }
        else if(!str.compare("ElementDataFile"))
        {
            fileStream >> str2; // '='
            fileStream >> imageFilename;
        }
        else if(!str.compare("NDims"))
        {
            fileStream >> str2;  // '='
            fileStream >> nbdims;
            if(nbdims>4) { std::cout << "MetaImageReader: dimensions > 4 not supported  "<<std::endl; return ret;}
        }
        else if(!str.compare("ElementNumberOfChannels"))
        {
            fileStream >> str2;  // '='
            fileStream >> nbchannels;
        }
        else if(!str.compare("DimSize") || !str.compare("dimensions") || !str.compare("dim"))
        {
            fileStream >> str2;  // '='
            for(unsigned int i=0;i<nbdims;i++) fileStream >> dim[i];
        }
        else if(!str.compare("ElementSpacing") || !str.compare("spacing") || !str.compare("scale3d") || !str.compare("voxelSize"))
        {
            fileStream >> str2; // '='
            double val[4];
            for(unsigned int i=0;i<nbdims;i++) fileStream >> val[i];
            if(scale) for(unsigned int i=0;i<3;i++) if(i<nbdims) scale[i] = (F)val[i];
            if(scaleT) if(nbdims>3) *scaleT = (F)val[3];
       }
        else if(!str.compare("Position") || !str.compare("Offset") || !str.compare("translation") || !str.compare("origin"))
        {
            fileStream >> str2; // '='
            double val[4];
            for(unsigned int i=0;i<nbdims;i++) fileStream >> val[i];
            if(translation) for(unsigned int i=0;i<3;i++) if(i<nbdims) translation[i] = (F)val[i];
            if(offsetT) if(nbdims>3) *offsetT = (F)val[3];
        }
        else if(!str.compare("Orientation") || !str.compare("TransformMatrix") || !str.compare("Rotation"))
        {
            fileStream >> str2; // '='
            double val[4*4];
            for(unsigned int i=0;i<nbdims*nbdims;i++) fileStream >> val[i];
            if(affine) { for(unsigned int i=0;i<3;i++) if(i<nbdims) for(unsigned int j=0;j<3;j++) if(j<nbdims) affine[i*3+j] = (F)val[i*nbdims+j]; }
            // to do: handle "CenterOfRotation" Tag
        }
        else if(!str.compare("isPerpective")) { fileStream >> str2; int val; fileStream >> val;  if(isPerspective) *isPerspective=val; }
        else if(!str.compare("ElementType") || !str.compare("voxelType"))  // not used (should be known in advance for template)
        {
            fileStream >> str2; // '='
            fileStream >> str2;

            if(!str2.compare("MET_CHAR"))           inputType=std::string("char");
            else if(!str2.compare("MET_DOUBLE"))    inputType=std::string("double");
            else if(!str2.compare("MET_FLOAT"))     inputType=std::string("float");
            else if(!str2.compare("MET_INT"))       inputType=std::string("int");
            else if(!str2.compare("MET_LONG"))      inputType=std::string("long");
            else if(!str2.compare("MET_SHORT"))     inputType=std::string("short");
            else if(!str2.compare("MET_UCHAR"))     inputType=std::string("unsigned char");
            else if(!str2.compare("MET_UINT"))      inputType=std::string("unsigned int");
            else if(!str2.compare("MET_ULONG"))     inputType=std::string("unsigned long");
            else if(!str2.compare("MET_USHORT"))    inputType=std::string("unsigned short");
            else if(!str2.compare("MET_BOOL"))      inputType=std::string("bool");

            if(inputType!=std::string(cimg::type<T>::string()))  std::cout<<"MetaImageReader: Image type ( "<< str2 <<" ) is converted to Image type ( "<< cimg::type<T>::string() <<" )"<<std::endl;
        }
    }
    fileStream.close();

    if(!imageFilename.size()) // no specified file name -> replace .mhd by .raw
    {
        imageFilename = std::string(headerFilename);
        imageFilename .replace(imageFilename.find_last_of('.')+1,imageFilename.size(),"raw");
    }
    else // add path to the specified file name
    {
        std::string tmp(headerFilename);
        std::size_t pos=tmp.find_last_of('/');
        if(pos==std::string::npos) pos=tmp.find_last_of('\\');
        if(pos!=std::string::npos) {tmp.erase(pos+1); imageFilename.insert(0,tmp);}
    }

    ret.assign(dim[3],dim[0],dim[1],dim[2],nbchannels);
    unsigned int nb = dim[0]*dim[1]*dim[2]*nbchannels;
    std::FILE *const nfile = std::fopen(imageFilename.c_str(),"rb");
    if(!nfile) return ret;

    if(inputType==std::string(cimg::type<T>::string()))
    {
        cimglist_for(ret,l)  cimg::fread(ret(l)._data,nb,nfile);
    }
    else
    {
        if(inputType==std::string("char"))
        {
            char *const buffer = new char[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("double"))
        {
            double *const buffer = new double[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("float"))
        {
            float *const buffer = new float[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("int"))
        {
            int *const buffer = new int[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("long"))
        {
            long *const buffer = new long[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("short"))
        {
            short *const buffer = new short[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("unsigned char"))
        {
            unsigned char *const buffer = new unsigned char[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("unsigned int"))
        {
            unsigned int *const buffer = new unsigned int[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("unsigned long"))
        {
            unsigned long *const buffer = new unsigned long[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("unsigned short"))
        {
            unsigned short *const buffer = new unsigned short[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
        else if(inputType==std::string("bool"))
        {
            bool *const buffer = new bool[dim[3]*nb];
            cimg::fread(buffer,dim[3]*nb,nfile);
            //if (endian) cimg::invert_endianness(buffer,dim[3]*nb);
            cimglist_for(ret,l) cimg_foroff(ret(l),off) ret(l)._data[off] = (T)(buffer[off+l*nb]);
            delete[] buffer;
        }
    }
    cimg::fclose(nfile);

    return ret;
}


#endif // CIMGTOOLS_H
