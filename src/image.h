#ifndef image_H
#define image_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "cimgTools.h"
#include <QFileInfo>

typedef float real;
static const real EPSILON=1E-10;
static const unsigned char LINECOLORS[3][3]={{ 255,0,0 },{ 0,255,0 },{ 0,0,255 }};
static const unsigned char BRUSHCOLOR[4]={255,0,0,255}; // BGRA
static const unsigned char ROICOLOR[3]={50,50,255};
static const unsigned char SEEDCOLOR[3]={0,255,0};
static const unsigned char LANDMARKCOLOR[3]={255,100,0};
static const unsigned char SELECTEDLANDMARKCOLOR[3]={100,255,0};
static const unsigned int MAXLABELS = 256;

class  MESH
{
public:
    CImgList<unsigned int> faces;
    CImg<float> vertices;
    CImg<float> normals;
    double center[3];

    bool save(const std::string& objFile)
    {
        if(!vertices.width()) return false;

        std::ofstream objStream (objFile.c_str());
        if(!objStream.is_open()) return false;
        cimg_forX(vertices,j) objStream<<"v "<<vertices(j,0)<<" "<<vertices(j,1)<<" "<<vertices(j,2)<<std::endl;
        cimglist_for(faces,l)
        {
            objStream<<"f";
            cimg_forY(faces(l),j)  objStream<<" "<<(1+faces(l)(j));
            objStream<<std::endl;
        }
        objStream.close();
        return true;
    }

    void update()
    {
        center[0]=0;    center[1]=0;    center[2]=0;
        if(!vertices.width()) return;
        cimg_forXY(vertices,j,k) center[k]+=vertices(j,k);
        center[0]/=(double)vertices.width();    center[1]/=(double)vertices.width();    center[2]/=(double)vertices.width();
    }
};


class LANDMARK
{
public:
    LANDMARK(const std::string n=std::string("new")):name(n) { for(unsigned int i=0;i<3;++i)        position[i]=0; }
    LANDMARK(const real p[3],const std::string n=std::string("new")):name(n) { for(unsigned int i=0;i<3;++i)        position[i]=p[i]; }
    ~LANDMARK()    {}
    real position[3];
    std::string name;
};

template<typename T>
class image
{
public:
    enum Selection { IMAGE, GRADX, GRADY, GRADZ, GRADNORM, DISPX, DISPY, FFTZ };
    
    CImg<T> img;
    
    CImg<bool> roi;
    
    real voxelSize[3];
    unsigned int dim[3];
    real translation[3];
    real rotation[3][3];

    int coord[3]; // 3D coordinates corresponding to mouse position
    int slice[3]; // current slices of the mpr visualisation
    unsigned int viewBB[2][3]; // bounding box of the zoomed region
    unsigned int dimBB[3]; // dimension of zoomed region
    int seed[4]; // 3D coordinates corresponding to seed position for region growing + area (for 2D region growing)
    int brushSize;
    T intensityRange[2];

    CImg<unsigned char> label;
    CImg<unsigned char> label_backup;
    std::vector<std::string> labelName;
    std::vector<bool> labelLock;
    CImg<unsigned char> palette;

    std::string currentPath;
    std::string imageFileName;
    std::string labelFileName;

    // meshes
    std::vector<MESH> meshes;

    // landmarks
    std::vector<LANDMARK> landmarks;
    int selectedLandmark;
    std::string landmarksFileName;

    image()
    {
        for(unsigned int i=0;i<3;++i)   for(unsigned int j=0;j<3;++j) rotation[i][j]=0;
        for(unsigned int i=0;i<3;++i)      { slice[i]=coord[i]=dim[i]=translation[i]=dimBB[i]=viewBB[0][i]=viewBB[1][i]=seed[i]=0; voxelSize[i]=rotation[i][i]=1.; }
        seed[3]=2;
        meshes.resize(MAXLABELS);
        labelLock.resize(MAXLABELS); for(unsigned int i=0;i<labelLock.size();++i) labelLock[i]=false;
        labelName.resize(MAXLABELS); for(unsigned int i=0;i<labelName.size();++i) { std::stringstream ss; ss<<i; labelName[i]=std::string("label ")+ss.str(); }
        CImg<int> tmp(labelName.size(),1,1,3,1);
        for(unsigned int i=0;i<labelName.size();++i) tmp(i)=(i+ i*25) % 359;
        //tmp.get_shared_channel(0).rand(0,359);
        palette = tmp.HSVtoRGB();
        brushSize=10;
        intensityRange[0]=intensityRange[1]=0;
        selectedLandmark=-1;
    }
    
    ~image()
    {
        
    }
    
    bool loadImage(const char* filename)
    {
        if(fopen(filename, "r")==NULL) return false;
        for(unsigned int i=0;i<3;++i)   for(unsigned int j=0;j<3;++j) rotation[i][j]=0;
        for(unsigned int i=0;i<3;++i)      { translation[i]=0; rotation[i][i]=1.; }

        std::string file(filename);
        if (file.find(".hdr")!=std::string::npos)        img.load_analyze(filename,voxelSize);
        else if(file.find(".mhd")!=std::string::npos || file.find(".MHD")!=std::string::npos || file.find(".Mhd")!=std::string::npos
                || file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)
        {
            if(file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)      file.replace(file.find_last_of('.')+1,file.size(),"mhd");
            img=load_metaimage<T,real>(file.c_str(),voxelSize,translation,&(rotation[0][0]))(0);
        }
        else if (file.find(".inr")!=std::string::npos)
        {
            float voxsize[3];
            img.load_inr(file.c_str(),voxsize);
            for(unsigned int i=0;i<3;i++) voxelSize[i]=(real)voxsize[i];
        }
        else img.load(file.c_str());
        
        if(img.is_empty()) return false;
        
        dim[0]=img.width(); dim[1]=img.height(); dim[2]=img.depth();
        for(unsigned int i=0;i<3;++i)     {   slice[i]=coord[i]=seed[i]=dim[i]/2;  }
        seed[3]=2;
        resetViewBB();
        
        roi.resize(dim[0],dim[1],dim[2],1); roi.fill(0);
        if((int)dim[0]!=label.width() || (int)dim[1]!=label.height() || (int)dim[2]!=label.depth())
        {
            label.resize(dim[0],dim[1],dim[2],1); label.fill(0); label_backup=label;
            qDebug()<<"clean existing labels";
        }
        
        intensityRange[0]=img.min();
        intensityRange[1]=img.max();

        imageFileName=std::string(filename);
        QFileInfo fileInfo(QString(imageFileName.c_str()));         currentPath=fileInfo.path().toStdString();

        qDebug()<<"dim:"<<dim[0]<<","<<dim[1]<<","<<dim[2];
        qDebug()<<"voxelSize:"<<voxelSize[0]<<","<<voxelSize[1]<<","<<voxelSize[2];
        qDebug()<<"translation:"<<translation[0]<<","<<translation[1]<<","<<translation[2];
        qDebug()<<"rotation:"<<rotation[0][0]<<","<<rotation[0][1]<<","<<rotation[0][2]<<","<<rotation[1][0]<<","<<rotation[1][1]<<","<<rotation[1][2]<<","<<rotation[2][0]<<","<<rotation[2][1]<<","<<rotation[2][2];
        return true;
    }


    bool saveImage(const char* filename)
    {
        if(img.is_empty()) return false;

        std::string file(filename);
        if (file.find(".hdr")!=std::string::npos)        img.save_analyze(filename,voxelSize);
        else if(file.find(".mhd")!=std::string::npos || file.find(".MHD")!=std::string::npos || file.find(".Mhd")!=std::string::npos
                || file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)
        {
            if(file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)      file.replace(file.find_last_of('.')+1,file.size(),"mhd");
            CImgList<T> l; l.push_back(img);
            save_metaimage<T,real>(l,filename,voxelSize,translation,&(rotation[0][0]));
        }

        imageFileName=file;
        QFileInfo fileInfo(QString(imageFileName.c_str()));         currentPath=fileInfo.path().toStdString();
        return true;
    }
    
    bool loadLabel(const char* filename)
    {
        if(img.is_empty()) return false;
        if(fopen(filename, "r")==NULL) return false;
        
        std::string file(filename);

        if(file.find(".mhd")!=std::string::npos || file.find(".MHD")!=std::string::npos || file.find(".Mhd")!=std::string::npos)
            //                || file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)
        {
            //           if(file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)      file.replace(file.find_last_of('.')+1,file.size(),"mhd");
            real voxelSize2[3],translation2[3],rotation2[3][3];
            label=load_metaimage<unsigned char,real>(file.c_str(),voxelSize2,translation2,&(rotation2[0][0]))(0);
            if(label.width()!=(int)dim[0] || label.height()!=(int)dim[1] || label.depth()!=(int)dim[2])
            {
                qDebug()<<"warning: resample and transform loaded labels to fit current image";
                CImg<unsigned char> newlabel(dim[0],dim[1],dim[2]); newlabel.fill(0);
                real p[3];
                cimg_forXYZ(newlabel,x,y,z)
                {
                    int P[3]={x,y,z};
                    imageCoordToPos(p,P);
                    // posToImageCoord for loaded image
                    for(unsigned int i=0;i<3;++i)
                    {
                        real val=0;
                        for(unsigned int j=0;j<3;j++) val+=rotation2[j][i]*(p[j]-translation2[j]);
                        val/=voxelSize2[i];
                        P[i]=round(val);
                    }
                    if(label.containsXYZC(P[0],P[1],P[2])) newlabel(x,y,z)=label(P[0],P[1],P[2]);
                }
                label=newlabel;
            }
        }
        else if (file.find(".raw")!=std::string::npos)        label.load_raw(file.c_str(),dim[0],dim[1],dim[2]);
        else if (file.find(".hdr")!=std::string::npos)             label.load_analyze(file.c_str());
        else return false;
        
        if(label.is_empty()) return false;
        labelFileName=std::string(filename);
        label_backup=label;

        for(unsigned int i=0;i<meshes.size();++i) {meshes[i].vertices.assign(); meshes[i].faces.assign();}
        return true;
    }
    
    bool saveLabel(const char* filename)
    {
        if(label.is_empty()) return false;

        std::string file(filename);
        if (file.find(".hdr")!=std::string::npos)        label.save_analyze(filename,voxelSize);
        else if(file.find(".mhd")!=std::string::npos || file.find(".MHD")!=std::string::npos || file.find(".Mhd")!=std::string::npos
                || file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)
        {
            if(file.find(".raw")!=std::string::npos || file.find(".RAW")!=std::string::npos || file.find(".Raw")!=std::string::npos)      file.replace(file.find_last_of('.')+1,file.size(),"mhd");
            CImgList<unsigned char> l; l.push_back(label);
            save_metaimage<unsigned char,real>(l,filename,voxelSize,translation,&(rotation[0][0]));
        }

        labelFileName=file;
        return true;
    }
    
    std::string getNamesFile()
    {
        if(labelFileName.length()==0) return NULL;
        std::string namesFile(labelFileName);
        // remove "_label"
        std::string from("_label");
        size_t start_pos = namesFile.find(from);
        if(start_pos != std::string::npos)   namesFile.replace(start_pos, from.length(), std::string());
        // add _names.txt
        namesFile.replace(namesFile.begin()+namesFile.rfind('.'),namesFile.end(),"_names.txt");
        return namesFile;
    }

    bool loadNames(const char* filename=NULL)
    {
        std::string nameFile = filename?std::string(filename):getNamesFile();
        if(fopen(nameFile.c_str(), "r")==NULL) return false;
        std::ifstream iStream (nameFile.c_str(), std::ifstream::in);
        if(iStream.is_open())
        {
            char txt[1024]; int i;
            while(!iStream.eof()) { iStream >> i ; iStream.getline(txt,1024); if(!iStream.eof() && i>=0 && i<(int)labelName.size()) labelName[i]=std::string(txt+1);}
            iStream.close();
            for(unsigned int i=0;i<labelLock.size();++i) labelLock[i]=false;
            return true;
        }
        else return false;
    }
    
    bool saveNames(const char* filename=NULL)
    {
        std::string nameFile = filename?std::string(filename):getNamesFile();
        std::ofstream fileStream (nameFile.c_str());
        if(fileStream.is_open())
        {
            for(unsigned int i=0;i<labelName.size();i++) {fileStream<<i<<" "<<labelName[i].c_str()<<std::endl;}
            fileStream.close();
            return true;
        }
        else return false;
    }

    bool loadLandmarks(const char* filename=NULL)
    {
        std::string nameFile;
        if(!filename) nameFile=landmarksFileName; else { nameFile=std::string(filename); landmarksFileName=nameFile; }
        if(fopen(nameFile.c_str(), "r")==NULL) return false;
        std::ifstream iStream (nameFile.c_str(), std::ifstream::in);
        if(iStream.is_open())
        {
            char txt[1024]; real p[3];
            landmarks.clear();
            while(!iStream.eof()) { iStream >> p[0]; iStream >> p[1]; iStream >> p[2]; iStream.getline(txt,1024); if(!iStream.eof()) landmarks.push_back(LANDMARK(p,std::string(txt+1)));}
            iStream.close();
            return true;
        }
        else return false;
    }

    bool saveLandmarks(const char* filename=NULL)
    {
        std::string nameFile;
        if(!filename) nameFile=landmarksFileName; else { nameFile=std::string(filename); landmarksFileName=nameFile; }
        std::ofstream fileStream (nameFile.c_str());
        if(fileStream.is_open())
        {
            for(unsigned int i=0;i<landmarks.size();i++) {fileStream<<landmarks[i].position[0]<<" "<<landmarks[i].position[1]<<" "<<landmarks[i].position[2]<<" "<<landmarks[i].name.c_str()<<std::endl;}
            fileStream.close();
            return true;
        }
        else return false;
    }




    template<class t>
    void imageCoordToPos(real p[3],const t p0[3]) const
    {
        for(unsigned int i=0;i<3;++i)
        {
            p[i]=translation[i];
            for(unsigned int j=0;j<3;j++)
                p[i]+=rotation[i][j]*(real)p0[j]*voxelSize[j];

        }
    }

    template<class t>
    void posToImageCoord(t p[3],const real p0[3]) const
    {
        for(unsigned int i=0;i<3;++i)
        {
            real P=0;
            for(unsigned int j=0;j<3;j++)
                P+=rotation[j][i]*(p0[j]-translation[j]);
            P/=voxelSize[i];
            p[i]=(t)P; // todo round
        }
    }

    bool crop()
    {
        if(img.is_empty()) return false;
        img.crop(viewBB[0][0],viewBB[0][1],viewBB[0][2],viewBB[1][0],viewBB[1][1],viewBB[1][2]);
        if(!label.is_empty()) label.crop(viewBB[0][0],viewBB[0][1],viewBB[0][2],viewBB[1][0],viewBB[1][1],viewBB[1][2]);
        label_backup=label;
        if(!roi.is_empty()) roi.crop(viewBB[0][0],viewBB[0][1],viewBB[0][2],viewBB[1][0],viewBB[1][1],viewBB[1][2]);

        dim[0]=img.width(); dim[1]=img.height(); dim[2]=img.depth();

        imageCoordToPos(translation,viewBB[0]);

        for(unsigned int i=0;i<3;++i)     {   slice[i]-=viewBB[0][i]; coord[i]-=viewBB[0][i]; seed[i]-=viewBB[0][i];  }
        resetViewBB();

        T m=img.min(); if(intensityRange[0]<m) intensityRange[0]=m;
        T M=img.max(); if(intensityRange[1]>M) intensityRange[1]=M;
        return true;
    }

    bool resampleDim(const unsigned int newDim[3])
    {
        if(img.is_empty()) return false;
        if(newDim[0]==dim[0] && newDim[1]==dim[1] && newDim[2]==dim[2]) return false;
        img.resize(newDim[0],newDim[1],newDim[2],-100,3);

        // clean resize through thresholding of a linearly interpolated image
        CImg<float> roif(roi);
        roif.resize(newDim[0],newDim[1],newDim[2],-100,3);
        roi.resize(newDim[0],newDim[1],newDim[2],-100,1);
        cimg_foroff(roif,off) if(roif[off]>0.5) roi[off]=true; else roi[off]=false;

        CImg<unsigned char> newlabel(newDim[0],newDim[1],newDim[2]); newlabel.fill(0);
        for(unsigned int i=1;i<MAXLABELS;i++) // ignore exterior
        {
            bool empty=true;
            CImg<float> labelf(label.width(),label.height(),label.depth(),label.spectrum(),0.0);
            cimg_forXYZ(label,x,y,z) if(label(x,y,z)==(unsigned char)i) { labelf(x,y,z)=1.0; empty=false; }
            if(!empty)
            {
                labelf.resize(newDim[0],newDim[1],newDim[2],-100,3);
                cimg_foroff(labelf,off) if(labelf[off]>0.5) newlabel[off]=(unsigned char)i;
            }
        }
        label=newlabel;
        label_backup=newlabel;

        T m=img.min(); if(intensityRange[0]<m) intensityRange[0]=m;
        T M=img.max(); if(intensityRange[1]>M) intensityRange[1]=M;
        real t[3],newt[3];
        for(unsigned int i=0;i<3;i++)
        {
            voxelSize[i]*=(real)dim[i]/(real)newDim[i];
            slice[i]=floor((real)slice[i]*(real)newDim[i]/(real)dim[i]);
            coord[i]=floor((real)coord[i]*(real)newDim[i]/(real)dim[i]);
            seed[i]=floor((real)seed[i]*(real)newDim[i]/(real)dim[i]);
            //            viewBB[0][i]=floor((real)viewBB[0][i]*(real)newDim[i]/(real)dim[i]);
            //            viewBB[1][i]=floor((real)viewBB[1][i]*(real)newDim[i]/(real)dim[i]);
            dimBB[i]=viewBB[1][i]-viewBB[0][i]+1;
            t[i]=(real)0.5*(1.-(real)newDim[i]/(real)dim[i]);
        }
        imageCoordToPos(newt,t);
        for(unsigned int i=0;i<3;i++) translation[i]=newt[i];

        dim[0]=img.width(); dim[1]=img.height(); dim[2]=img.depth();
        resetViewBB();

        return true;
    }

    bool resampleVoxelSize(const real newVSize[3])
    {
        unsigned int newDim[3];
        for(unsigned int i=0;i<3;i++) newDim[i]=round(voxelSize[i]*(real)dim[i]/newVSize[i]);
        return resampleDim(newDim);
    }

    bool mirror(const char axes)
    {
        if(img.is_empty()) return false;
        img.mirror(axes);
        roi.mirror(axes);
        label.mirror(axes);
        return true;
    }

    void undo()
    {
        label_backup.swap(label);
    }

    void getPlaneDirections(int dir[2],const unsigned int area)
    {
        if(area==0)      {dir[0]=2; dir[1]=1; } // ZY
        else if(area==1) {dir[0]=0; dir[1]=2; } // XZ
        else {dir[0]=0; dir[1]=1; } // XY
    }
    
    void getIntensityRangeLimits(T& _min,T& _max)
    {
        if(img.is_empty()) return;
        _min = img.min();
        _max = img.max();
    }
    
    CImg<unsigned char> getEmptyCutplane(const unsigned int area)
    {
        // initialize an image to be used for overlay (4 channels RGBA)
        if(img.is_empty()) return CImg<unsigned char>();
        int dir[2]; getPlaneDirections(dir,area);
        CImg<unsigned char> im(dimBB[dir[0]],dimBB[dir[1]],1,4);
        im.fill(0);
        return im;
    }
    
    CImg<unsigned char> getCutplane(const unsigned int area)
    {
        if(img.is_empty()) return CImg<unsigned char>();
        
        int P[3],dir[2]; getPlaneDirections(dir,area);
        CImg<T> p(dimBB[dir[0]],dimBB[dir[1]],1,3);
        P[area]=slice[area];
        cimg_forXY(p,x,y)
        {
            P[dir[0]]=x+viewBB[0][dir[0]];
            P[dir[1]]=y+viewBB[0][dir[1]];
            p(x,y,0,0)=p(x,y,0,1)=p(x,y,0,2)=img(P[0],P[1],P[2]);
        }
        CImg<unsigned char> r=p.get_cut(intensityRange[0],intensityRange[1]).normalize(0,255);
        return r;
    }
    
    void overlayLabels(CImg<unsigned char>& cutplane,const unsigned int area, const double opacity=0.2, const bool borderOnly=true,const double roiopacity=0.2)
    {
        if(label.is_empty()) return;
        if(roi.is_empty()) return;
        if(opacity==0) return;
        
        int P[3],dir[2]; getPlaneDirections(dir,area);
        CImg<unsigned char> l(dimBB[dir[0]],dimBB[dir[1]]); l.fill(0);
        CImg<bool> r(dimBB[dir[0]],dimBB[dir[1]]); r.fill(0);
        P[area]=slice[area];
        cimg_forXY(l,x,y)
        {
            P[dir[0]]=x+viewBB[0][dir[0]];
            P[dir[1]]=y+viewBB[0][dir[1]];
            l(x,y)=label(P[0],P[1],P[2]);
            r(x,y)=roi(P[0],P[1],P[2]);
        }
        
        if(borderOnly)
        {
            CImg<> tmp=l;
            CImg_3x3(I,unsigned char);
            cimg_for3x3(l,x,y,0,0,I,unsigned char)
                    if(Icc)
                    if(Ipc==Icc && Inc==Icc && Icn==Icc && Icp==Icc)
                    //if(Ipp==Icc && Inp==Icc && Ipn==Icc && Inn==Icc)
                    tmp(x,y)=0;
            l=tmp;
            
        }
        //blend
        cimg_forXY(l,x,y) if(r(x,y))      cimg_forC(cutplane,c) cutplane(x,y,0,c) = (1.-roiopacity)*cutplane(x,y,0,c)+roiopacity*ROICOLOR[c];
        else if(l(x,y)) cimg_forC(cutplane,c) cutplane(x,y,0,c) = (1.-opacity)*cutplane(x,y,0,c)+opacity*palette(l(x,y),0,0,c);
    }
    
    void overlaySliceTrace(CImg<unsigned char>& cutplane,const unsigned int area, const unsigned int axis)
    {
        int dir[2]; getPlaneDirections(dir,area);
        int x=slice[dir[axis]]-viewBB[0][dir[axis]];
        if(axis==0) cutplane.draw_line(x,0,0,x,cutplane.height()-1,0,LINECOLORS[dir[axis]]);
        else cutplane.draw_line(0,x,0,cutplane.width()-1,x,0,LINECOLORS[dir[axis]]);
    }
    
    void overlayBrush(CImg<unsigned char>& cutplane,const unsigned int area)
    {
        int dir[2]; getPlaneDirections(dir,area);
        int x=coord[dir[0]]-viewBB[0][dir[0]],y=coord[dir[1]]-viewBB[0][dir[1]];
        cutplane.draw_circle(x,y,brushSize,BRUSHCOLOR,1.0f,~0U);
    }
    
    void overlaySeed(CImg<unsigned char>& cutplane,const unsigned int area,const unsigned int size=2)
    {
        if(seed[area]!=slice[area]) return;
        int dir[2]; getPlaneDirections(dir,area);
        int x=seed[dir[0]]-viewBB[0][dir[0]],y=seed[dir[1]]-viewBB[0][dir[1]];
        cutplane.draw_line(x-size,y,0,x-1,y,0,SEEDCOLOR)
                .draw_line(x+1,y,0,x+size,y,0,SEEDCOLOR)
                .draw_line(x,y-size,0,x,y-1,0,SEEDCOLOR)
                .draw_line(x,y+1,0,x,y+size,0,SEEDCOLOR);
    }
    
    void overlayLandmarks(CImg<unsigned char>& cutplane,const unsigned int area,const unsigned int size=2)
    {
        int dir[2]; getPlaneDirections(dir,area);
        int p[3];
        for(unsigned int i=0;i<landmarks.size();i++)
        {
            posToImageCoord(p,landmarks[i].position);
            if(p[area]!=(int)slice[area]) continue;
            int x=p[dir[0]]-viewBB[0][dir[0]],y=p[dir[1]]-viewBB[0][dir[1]];
            if((int)i==selectedLandmark)
                cutplane.draw_line(x-size,y,0,x-1,y,0,SELECTEDLANDMARKCOLOR)
                        .draw_line(x+1,y,0,x+size,y,0,SELECTEDLANDMARKCOLOR)
                        .draw_line(x,y-size,0,x,y-1,0,SELECTEDLANDMARKCOLOR)
                        .draw_line(x,y+1,0,x,y+size,0,SELECTEDLANDMARKCOLOR);
            else
                cutplane.draw_line(x-size,y,0,x-1,y,0,LANDMARKCOLOR)
                        .draw_line(x+1,y,0,x+size,y,0,LANDMARKCOLOR)
                        .draw_line(x,y-size,0,x,y-1,0,LANDMARKCOLOR)
                        .draw_line(x,y+1,0,x,y+size,0,LANDMARKCOLOR);
        }
    }

    void selectBrush(const unsigned int area,const bool add)
    {
        int dir[2]; getPlaneDirections(dir,area);
        int x=coord[dir[0]],y=coord[dir[1]];
        bool tru=true;
        CImg<bool> tmp(dim[dir[0]],dim[dir[1]],1,1,false);
        tmp.draw_circle(x,y,brushSize,&tru);
        int P[3];
        P[area]=coord[area];
        cimg_forXY(tmp,X,Y)
                if(tmp(X,Y))
        {
            P[dir[0]]=X;
            P[dir[1]]=Y;
            if(!labelLock[label(P[0],P[1],P[2])]) roi(P[0],P[1],P[2])=add;
        }
    }
    
    void selectSeed(const unsigned int area)
    {
        for(unsigned int i=0;i<3;++i)  seed[i]=coord[i];
        seed[3]=area;
    }


    //,const CImgList<unsigned int> &separationPoints,const unsigned int separationLinkTol2
    void regionGrowing(const T range[2],  const bool inLabel,const bool connected,const bool is2D)
    {
        if(label.is_empty()) return;
        if(roi.is_empty()) return;

        if(is2D)
        {
            int P[3],dir[2]; getPlaneDirections(dir,seed[3]);
            P[seed[3]]=seed[seed[3]];
            CImg<T> c_img(dim[dir[0]],dim[dir[1]]);
            CImg<unsigned char> c_label(dim[dir[0]],dim[dir[1]]);
            CImg<bool> c_roi(dim[dir[0]],dim[dir[1]]); c_roi.fill(false);

            if(img(seed[0],seed[1],seed[2])<=range[1] && img(seed[0],seed[1],seed[2])>=range[0])
            {
                cimg_forXY(c_img,x,y)
                {
                    P[dir[0]]=x;
                    P[dir[1]]=y;
                    c_img(x,y)=img(P[0],P[1],P[2]);
                    c_label(x,y)=label(P[0],P[1],P[2]);
                }
                if(inLabel)
                {
                    T lab=(int)label(seed[0],seed[1],seed[2]);
                    cimg_forXY(c_img,x,y) if(c_label(x,y)==lab) if(c_img(x,y)<=range[1] && c_img(x,y)>=range[0]) c_roi(x,y)=true;
                }
                else
                {
                    cimg_forXY(c_img,x,y) if(c_img(x,y)<=range[1] && c_img(x,y)>=range[0]) c_roi(x,y)=true;
                }
                if(connected)
                {
                    bool tru=true;
                    (+c_roi).draw_fill(seed[dir[0]],seed[dir[1]],0,&tru,1.0f,c_roi,0);
                }
            }
            // paste
            cimg_forXY(c_roi,x,y)
            {
                P[dir[0]]=x;
                P[dir[1]]=y;

                if(!labelLock[label(P[0],P[1],P[2])] && c_roi(x,y)) roi(P[0],P[1],P[2])=true;
                else roi(P[0],P[1],P[2])=false;
            }

            //                CImg<bool> sepImg=get_plane(separationPoints,seed,seed[3],BB,separationLinkTol2);
            //                if(seed[3]==1)      {  cimg_forXY(sepImg,z,y) if(sepImg(z,y)) pselect(z,y,0)=false; (+pselect).draw_fill(seed[2],seed[1],0,&white,1.0f,pselect,0); }
            //                else if(seed[3]==2) {  cimg_forXY(sepImg,x,z) if(sepImg(x,z)) pselect(x,z,0)=false; (+pselect).draw_fill(seed[0],seed[2],0,&white,1.0f,pselect,0); }
            //                else if(seed[3]==3) {  cimg_forXY(sepImg,x,y) if(sepImg(x,y)) pselect(x,y,0)=false; (+pselect).draw_fill(seed[0],seed[1],0,&white,1.0f,pselect,0); }
        }
        else
        {
            roi.fill(false);
            if(img(seed[0],seed[1],seed[2])<=range[1] && img(seed[0],seed[1],seed[2])>=range[0])
                if(!labelLock[label(seed[0],seed[1],seed[2])])
                {
                    // todo: openmp ?
                    if(inLabel)
                    {
                        T lab=(int)label(seed[0],seed[1],seed[2]);
                        cimg_forXYZ(img,x,y,z) if(label(x,y,z)==lab) if(img(x,y,z)<=range[1] && img(x,y,z)>=range[0]) roi(x,y,z)=true;
                    }
                    else
                    {
                        cimg_forXYZ(img,x,y,z) if(img(x,y,z)<=range[1] && img(x,y,z)>=range[0]) roi(x,y,z)=true;
                    }
                    if(connected)
                    {
                        bool tru=true;
                        (+roi).draw_fill(seed[0],seed[1],seed[2],&tru,1.0f,roi,0);
                    }
                    unselectLockedLabels();
                }
        }
    }



    void clearRoi()
    {
        roi.fill(0);
    }

    void addLabelToRoi(const unsigned int l)
    {
        if(roi.is_empty()) return;
        if(label.is_empty()) return;
        cimg_forXYZ(label,x,y,z) if(label(x,y,z)==l) roi(x,y,z)=1;
    }

    void addRoiToLabel(const unsigned int l)
    {
        if(roi.is_empty()) return;
        if(label.is_empty()) return;
        if(labelLock[l]) return;
        label_backup=label;
        cimg_forXYZ(label,x,y,z) if(roi(x,y,z)==1) if(!labelLock[label(x,y,z)]) label(x,y,z)=l;
    }

    void delRoiFromLabel(const unsigned int l)
    {
        if(roi.is_empty()) return;
        if(label.is_empty()) return;
        if(labelLock[l]) return;
        label_backup=label;
        cimg_forXYZ(label,x,y,z) if(roi(x,y,z)==1 && label(x,y,z)==l) label(x,y,z)=0;
    }

    void updateCoord(const double mousePosX,const double mousePosY,const unsigned int area)
    {
        int dir[2]; getPlaneDirections(dir,area);
        coord[dir[0]] = mousePosX*dimBB[dir[0]]+viewBB[0][dir[0]];
        coord[dir[1]] = mousePosY*dimBB[dir[1]]+viewBB[0][dir[1]];
        coord[area] = slice[area];
        for(unsigned int i=0;i<3;++i) clampWithinBB(coord[i],i);
    }

    template<class t>
    void clampWithinBB(t& pos,const unsigned int axis)
    {
        if(pos<(t)viewBB[0][axis]) pos=(t)viewBB[0][axis];
        else if(pos>=(t)viewBB[1][axis]) pos=(t)viewBB[1][axis];
    }

    std::string getLabelAtCoord()
    {
        if(label.is_empty()) return std::string();
        if(!label.containsXYZC(coord[0],coord[1],coord[2])) return std::string();
        return labelName[label(coord[0],coord[1],coord[2])];
    }

    T getIntensityAtCoord()
    {
        if(img.is_empty()) return 0;
        if(!img.containsXYZC(coord[0],coord[1],coord[2])) return 0;
        return img(coord[0],coord[1],coord[2]);
    }

    bool setSlice(const int p[3])
    {
        bool changed=false;
        for(unsigned int i=0;i<3;++i) if(setSlice(p[i],i)) changed=true;
        return changed;
    }

    bool setSlice(const int d,const unsigned int area)
    {
        int oldval=slice[area];
        slice[area]=d;
        clampWithinBB(slice[area],area);
        coord[area] = slice[area];
        if(oldval==slice[area]) return false; else return true;
    }

    void getPlaneDim(unsigned int& dimz, unsigned int dimxy[2], const unsigned int area)
    {
        int dir[2]; getPlaneDirections(dir,area);
        dimxy[0]=dimBB[dir[0]];
        dimxy[1]=dimBB[dir[1]];
        dimz=dimBB[area];
    }


    void getPlaneSize(double &z, double xy[2], const unsigned int area)
    {
        int dir[2]; getPlaneDirections(dir,area);
        xy[0]=voxelSize[dir[0]]*(double)dimBB[dir[0]];
        xy[1]=voxelSize[dir[1]]*(double)dimBB[dir[1]];
        z=voxelSize[area]*(double)dimBB[area];
    }




    void selectViewBB(const double mouseX1,const double mouseY1,const double mouseX2,const double mouseY2, const unsigned int area)
    {
        double xmin=std::min(mouseX1,mouseX2),ymin=std::min(mouseY1,mouseY2),xmax=std::max(mouseX1,mouseX2),ymax=std::max(mouseY1,mouseY2);
        updateCoord(xmin,ymin,area); int coordMin[3]={coord[0],coord[1],coord[2]};
        updateCoord(xmax,ymax,area); int coordMax[3]={coord[0],coord[1],coord[2]};
        for(unsigned int i=0;i<3;++i) if(i!=area) { viewBB[0][i]=coordMin[i]; viewBB[1][i]=coordMax[i]; dimBB[i]=coordMax[i]-coordMin[i]+1; clampWithinBB(slice[i],i); }
    }

    void moveViewBB(const int dp[2],const unsigned int area)
    {
        int dir[2]; getPlaneDirections(dir,area);
        for(unsigned int i=0;i<2;++i)
            if(dp[i]<0)
            {
                viewBB[0][dir[i]]=(unsigned int)std::max((int)viewBB[0][dir[i]]+dp[i],0);
                viewBB[1][dir[i]]=viewBB[0][dir[i]]+dimBB[dir[i]]-1;
                clampWithinBB(slice[dir[i]],dir[i]);
            }
            else if(dp[i]>0)
            {
                viewBB[1][dir[i]]=(unsigned int)std::min((int)viewBB[1][dir[i]]+dp[i],(int)dim[dir[i]]-(int)1);
                viewBB[0][dir[i]]=viewBB[1][dir[i]]-dimBB[dir[i]]+1;
                clampWithinBB(slice[dir[i]],dir[i]);
            }
    }

    void resetViewBB()
    {
        for(unsigned int i=0;i<3;++i)  { viewBB[0][i]=0; viewBB[1][i]=dim[i]-1;  dimBB[i]=dim[i]; }
    }

    void resetViewBB(const unsigned int area)
    {
        for(unsigned int i=0;i<3;++i) if(i!=area) { viewBB[0][i]=0; viewBB[1][i]=dim[i]-1;  dimBB[i]=dim[i];  }
    }

    void unselectLockedLabels()
    {
        cimg_foroff(label,off) if(roi[off]) if(labelLock[label[off]]) roi[off]=false;
    }

    void dilate(const unsigned int size)
    {
        roi.dilate(size);
        unselectLockedLabels();
    }

    void erode(const unsigned int size)
    {
        roi.erode(size);
        unselectLockedLabels();
    }

    void smooth(const float sigma)
    {
        CImg<real> rroi (roi);
        rroi.blur(sigma);
        cimg_foroff(rroi,off) if( rroi[off]<=0.5 ) roi[off]=0; else roi[off]=1;
        unselectLockedLabels();
    }

    void FillHoles2D(const unsigned int area)
    {
        int P[3],dir[2]; getPlaneDirections(dir,area);
        CImg<unsigned char> sel(dim[dir[0]],dim[dir[1]]);
        P[area]=slice[area];
        cimg_forXY(sel,x,y)
        {
            P[dir[0]]=x;
            P[dir[1]]=y;
            sel(x,y)=roi(P[0],P[1],P[2]);
        }
        unsigned char fillColor = (unsigned char)2;
        sel.draw_fill(0,0,0,&fillColor); // flood fill from voxel (0,0,0)
        cimg_forXY(sel,x,y)
        {
            P[dir[0]]=x;
            P[dir[1]]=y;
            if(!labelLock[label(P[0],P[1],P[2])] && sel(x,y)!=2) roi(P[0],P[1],P[2])=true;
            else roi(P[0],P[1],P[2])=false;
        }
    }

    void FillHoles()
    {
        CImg<unsigned char> sel = roi;
        unsigned char fillColor = (unsigned char)2;
        sel.draw_fill(0,0,0,&fillColor); // flood fill from voxel (0,0,0)
        cimg_foroff(sel,off) if( sel[off]==2 ) roi[off]=0; else roi[off]=1;
        unselectLockedLabels();
    }

    void InvertROI2D(const unsigned int area)
    {
        int P[3],dir[2]; getPlaneDirections(dir,area);
        P[area]=slice[area];
        for(unsigned int y=0;y<dim[dir[1]];++y)
            for(unsigned int x=0;x<dim[dir[0]];++x)
            {
                P[dir[0]]=x;
                P[dir[1]]=y;
                if(!labelLock[label(P[0],P[1],P[2])] && !roi(P[0],P[1],P[2])) roi(P[0],P[1],P[2])=true;
                else roi(P[0],P[1],P[2])=false;
            }
    }

    void InvertROI()
    {
        cimg_foroff(roi,off) if( roi[off] ) roi[off]=0; else roi[off]=1;
        unselectLockedLabels();
    }



    void addLandmark()
    {
        std::stringstream ss; ss<<landmarks.size();
        real p[3]; imageCoordToPos(p,slice);
        landmarks.push_back(LANDMARK(p,std::string("landmark ")+ss.str()));
    }

    void delLandmark(const unsigned int index)
    {
        landmarks.erase(landmarks.begin()+index);
    }

    void selectLandmark(const int index=-1)
    {
        selectedLandmark=index;
        posToImageCoord(slice,landmarks[selectedLandmark].position);
        for(unsigned int i=0;i<3;++i)  clampWithinBB(slice[i],i);
    }

    void setLandmark()
    {
        if(selectedLandmark<0 || selectedLandmark>=(int)landmarks.size()) return;
        imageCoordToPos(landmarks[selectedLandmark].position,coord);
    }



    unsigned int marchingCubes(const int res=0)
    {
        unsigned int nbfaces=0;
        for(unsigned int i=1;i<MAXLABELS;i++) // ignore exterior
        {
            meshes[i].faces.assign();
            meshes[i].vertices.assign();

            CImg<bool> sel(label.width(),label.height(),label.depth(),label.spectrum(),false);
            cimg_forXYZ(label,x,y,z) if(label(x,y,z)==i) sel(x,y,z)=true;

            int r=res; if(r<=0) r=-100;
            meshes[i].vertices = sel.get_isosurface3d (meshes[i].faces, 1.0,r,r,r);
            // transform points
            cimg_forX(meshes[i].vertices,j)
            {
                real p[3],p0[3]={meshes[i].vertices(j,0),meshes[i].vertices(j,1),meshes[i].vertices(j,2)};
                imageCoordToPos(p,p0);
                for(unsigned int k=0;k<3;k++) meshes[i].vertices(j,k)=p[k];
            }
            // flip faces
            cimglist_for(meshes[i].faces,l) meshes[i].faces(l).mirror('y');
            nbfaces+=meshes[i].faces.size();
            meshes[i].update();
        }
        return nbfaces;
    }

    bool saveObjs(const std::string& path)
    {
        for(unsigned int i=1;i<MAXLABELS;i++) // ignore exterior
            meshes[i].save(path+std::string("/")+labelName[i]+std::string(".obj"));
        return true;
    }


};



#endif // image_H
