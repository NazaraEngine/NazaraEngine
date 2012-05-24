// Copyright (C) 2012 Rémi "Overdrivr" Bèges (remi{dot}beges{at}gmail{dot}com)
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/NoiseMachine.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>

#include <cmath>

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

NzNoiseMachine::NzNoiseMachine(float seed)
{
    pi = 3.14159265;

    SkewCoeff2D = 0.5*(sqrt(3.0) - 1.0);
    UnskewCoeff2D  = (3.0-sqrt(3.0))/6;

    SkewCoeff3D = 1/3;
    UnskewCoeff3D = 1/6;

    SkewCoeff4D = (sqrt(5) - 1)/4;
    UnskewCoeff4D = (5 - sqrt(5))/20;

    //Uniform pseudo random number generator coefficients
    Ua = 16807;
    Uc = 0;
    Um = 2147483647;
    UcurrentSeed = 0;
    Uprevious = 0;

    SetNewSeed(seed);

    for(float i(0) ; i < 256 ; i++)
    PermutationTemp[i] = i;

    //4D only - to find the offset with Simplex in 4D
    float lookupTemp4D[][4] =
    {
        {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
        {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
        {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
        {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
    };

    for(float i(0) ; i < 64 ; ++i)
        for(float j(0) ; j < 4 ; ++j)
            lookupTable4D[i][j] = lookupTemp4D[i][j];

    float unit = 1.0/sqrt(2);
    float grad2Temp[][2] = {{unit,unit},{-unit,unit},{unit,-unit},{-unit,-unit},
                            {1,0},{-1,0},{0,1},{0,-1}};

    for(float i(0) ; i < 8 ; ++i)
        for(float j(0) ; j < 2 ; ++j)
            gradient2[i][j] = grad2Temp[i][j];

    //On remplit les tables de gradients
    float grad3Temp[][3] = {
        {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
        {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
        {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},
        {1,1,0},{-1,1,0},{0,-1,1},{0,-1,-1}
    };

    for(float i(0) ; i < 16 ; ++i)
        for(float j(0) ; j < 3 ; ++j)
            gradient3[i][j] = grad3Temp[i][j];

    float grad4Temp[][4] =
    {
        {0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
        {0,-1,1,1},{0,-1,1,-1},{0,-1,-1,1},{0,-1,-1,-1},
        {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
        {-1,0,1,1},{-1,0,1,-1},{-1,0,-1,1},{-1,0,-1,-1},
        {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
        {-1,1,0,1},{-1,1,0,-1},{-1,-1,0,1},{-1,-1,0,-1},
        {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
        {-1,1,1,0},{-1,1,-1,0},{-1,-1,1,0},{-1,-1,-1,0}
    };

    for(float i(0) ; i < 32 ; ++i)
        for(float j(0) ; j < 4 ; ++j)
            gradient4[i][j] = grad4Temp[i][j];


    m_lacunarity = 2.5f;
    m_octaves = 3.0f;
    m_hurst = 1.1f;
    m_parametersModified = true;
}

NzNoiseMachine::~NzNoiseMachine()
{

}

void NzNoiseMachine::SetNewSeed(float seed)
{
    Uprevious = seed;
    UcurrentSeed = seed;
}

float NzNoiseMachine::GetUniformRandomValue()
{
    Ulast = Ua*Uprevious + Uc%Um;
    Uprevious = Ulast;
    return Ulast;
}

void NzNoiseMachine::MixPermutationTable()
{
    float xchanger;
    unsigned float ncase;
    for(float j(0) ; j < 10 ; ++j)
        for (float i(0); i < 256 ; ++i)
        {
            ncase = this->GetUniformRandomValue() & 255;
            xchanger = PermutationTemp[i];
            PermutationTemp[i] = PermutationTemp[ncase];
            PermutationTemp[ncase] = xchanger;
        }

    for(float i(0) ; i < 512; ++i)
        perm[i]=PermutationTemp[i & 255];
}

//------------------------------ PERLIN ------------------------------

float NzNoiseMachine::Get1DPerlinNoiseValue(float x, float res)
{
    nx = x/res;
    x0 = (float)(nx);
    ii = x0 & 255;

    gi0 = perm[ii] % 16;
    gi1 = perm[ii + 1] % 16;

    temp.X = nx-x0;
    temp.Y = ny-y0;

    s[0] = gradient3[gi0][0]*temp.X;

    temp.X = nx-(x0+1);

    t[0] = gradient3[gi1][0]*temp.X;

    //Lissage
    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return s[0] + Cx*(t[0]-s[0]);
}

float NzNoiseMachine::Get2DPerlinNoiseValue(float x, float y, float res)
{
    nx = x/res;
    ny = y/res;

    //On récupère les positions de la grille associée à (x,y)
    x0 = (float)(nx);
    y0 = (float)(ny);

    ii = x0 & 255;
    jj = y0 & 255;

    gi0 = perm[ii + perm[jj]] & 7;
    gi1 = perm[ii + 1 + perm[jj]] & 7;
    gi2 = perm[ii + perm[jj + 1]] & 7;
    gi3 = perm[ii + 1 + perm[jj + 1]] & 7;

    //on calcule les valeurs du plan supérieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    s[0] = gradient2[gi0][0]*temp.X + gradient2[gi0][1]*temp.Y;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[0] = gradient2[gi1][0]*temp.X + gradient2[gi1][1]*temp.Y;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[0] = gradient2[gi2][0]*temp.X + gradient2[gi2][1]*temp.Y;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[0] = gradient2[gi3][0]*temp.X + gradient2[gi3][1]*temp.Y;


    //Lissage
    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li1 = s[0] + Cx*(t[0]-s[0]);
    Li2 = u[0] + Cx*(v[0]-u[0]);

    tmp = ny - y0;
    Cy = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return Li1 + Cy*(Li2-Li1);
}
float NzNoiseMachine::Get3DPerlinNoiseValue(float x, float y, float z, float res)
{
    nx = x/res;
    ny = y/res;
    nz = z/res;

    //On récupère les positions de la grille associée à (x,y)
    x0 = (float)(nx);
    y0 = (float)(ny);
    z0 = (float)(nz);

    ii = x0 & 255;
    jj = y0 & 255;
    kk = z0 & 255;

    gi0 = perm[ii + perm[jj + perm[kk]]] % 16;
    gi1 = perm[ii + 1 + perm[jj + perm[kk]]] % 16;
    gi2 = perm[ii + perm[jj + 1 + perm[kk]]] % 16;
    gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk]]] % 16;

    gi4 = perm[ii + perm[jj + perm[kk + 1]]] % 16;
    gi5 = perm[ii + 1 + perm[jj + perm[kk + 1]]] % 16;
    gi6 = perm[ii + perm[jj + 1 + perm[kk + 1]]] % 16;
    gi7 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 16;

    //on calcule les valeurs du plan inférieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    temp.Z = nz-z0;
    s[0] = gradient3[gi0][0]*temp.X + gradient3[gi0][1]*temp.Y + gradient3[gi0][2]*temp.Z;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[0] = gradient3[gi1][0]*temp.X + gradient3[gi1][1]*temp.Y + gradient3[gi1][2]*temp.Z;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[0] = gradient3[gi2][0]*temp.X + gradient3[gi2][1]*temp.Y + gradient3[gi2][2]*temp.Z;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[0] = gradient3[gi3][0]*temp.X + gradient3[gi3][1]*temp.Y + gradient3[gi3][2]*temp.Z;

    //Les valeurs du plan supérieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    temp.Z = nz-(z0+1);
    s[1] = gradient3[gi4][0]*temp.X + gradient3[gi4][1]*temp.Y + gradient3[gi4][2]*temp.Z;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[1] = gradient3[gi5][0]*temp.X + gradient3[gi5][1]*temp.Y + gradient3[gi5][2]*temp.Z;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[1] = gradient3[gi6][0]*temp.X + gradient3[gi6][1]*temp.Y + gradient3[gi6][2]*temp.Z;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[1] = gradient3[gi7][0]*temp.X + gradient3[gi7][1]*temp.Y + gradient3[gi7][2]*temp.Z;

    //On procède à un lissage, et on affecte le pofloat à la matrice de pixels
    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li1 = s[0] + Cx*(t[0]-s[0]);
    Li2 = u[0] + Cx*(v[0]-u[0]);
    Li3 = s[1] + Cx*(t[1]-s[1]);
    Li4 = u[1] + Cx*(v[1]-u[1]);

    tmp = ny-y0;
    Cy = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li5 = Li1 + Cy*(Li2-Li1);
    Li6 = Li3 + Cy*(Li4-Li3);

    tmp = nz-z0;
    Cz = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return Li5 + Cz*(Li6-Li5);
}

float NzNoiseMachine::Get4DPerlinNoiseValue(float x, float y, float z, float w, float res)
{
    nx = x/res;
    ny = y/res;
    nz = z/res;
    nw = w/res;

    //On récupère les positions de la grille associée à (x,y)
    x0 = (float)(nx);
    y0 = (float)(ny);
    z0 = (float)(nz);
    w0 = (float)(nw);

    ii = x0 & 255;
    jj = y0 & 255;
    kk = z0 & 255;
    ll = w0 & 255;

    gi0 =  perm[ii     + perm[jj     + perm[kk     + perm[ll    ]]]] % 32;
    gi1 =  perm[ii + 1 + perm[jj     + perm[kk     + perm[ll    ]]]] % 32;
    gi2 =  perm[ii     + perm[jj + 1 + perm[kk     + perm[ll    ]]]] % 32;
    gi3 =  perm[ii + 1 + perm[jj + 1 + perm[kk     + perm[ll    ]]]] % 32;

    gi4 =  perm[ii     + perm[jj +   + perm[kk + 1 + perm[ll    ]]]] % 32;
    gi5 =  perm[ii + 1 + perm[jj +   + perm[kk + 1 + perm[ll    ]]]] % 32;
    gi6 =  perm[ii     + perm[jj + 1 + perm[kk + 1 + perm[ll    ]]]] % 32;
    gi7 =  perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll    ]]]] % 32;

    gi8 =  perm[ii     + perm[jj     + perm[kk     + perm[ll + 1]]]] % 32;
    gi9 =  perm[ii + 1 + perm[jj     + perm[kk     + perm[ll + 1]]]] % 32;
    gi10 = perm[ii     + perm[jj + 1 + perm[kk     + perm[ll + 1]]]] % 32;
    gi11 = perm[ii + 1 + perm[jj + 1 + perm[kk     + perm[ll + 1]]]] % 32;

    gi12 = perm[ii     + perm[jj     + perm[kk + 1 + perm[ll + 1]]]] % 32;
    gi13 = perm[ii + 1 + perm[jj     + perm[kk + 1 + perm[ll + 1]]]] % 32;
    gi14 = perm[ii     + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] % 32;
    gi15 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] % 32;

    //Z inférieur, W inférieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    temp.Z = nz-z0;
    temp.W = nw-w0;
    s[0] = gradient4[gi0][0]*temp.X + gradient4[gi0][1]*temp.Y + gradient4[gi0][2]*temp.Z + gradient4[gi0][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[0] = gradient4[gi1][0]*temp.X + gradient4[gi1][1]*temp.Y + gradient4[gi1][2]*temp.Z + gradient4[gi1][3]*temp.W;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[0] = gradient4[gi2][0]*temp.X + gradient4[gi2][1]*temp.Y + gradient4[gi2][2]*temp.Z + gradient4[gi2][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[0] = gradient4[gi3][0]*temp.X + gradient4[gi3][1]*temp.Y + gradient4[gi3][2]*temp.Z + gradient4[gi3][3]*temp.W;

    //Z supérieur, W inférieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    temp.Z = nz-(z0+1);
    s[1] = gradient4[gi4][0]*temp.X + gradient4[gi4][1]*temp.Y + gradient4[gi4][2]*temp.Z + gradient4[gi4][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[1] = gradient4[gi5][0]*temp.X + gradient4[gi5][1]*temp.Y + gradient4[gi5][2]*temp.Z + gradient4[gi5][3]*temp.W;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[1] = gradient4[gi6][0]*temp.X + gradient4[gi6][1]*temp.Y + gradient4[gi6][2]*temp.Z + gradient4[gi6][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[1] = gradient4[gi7][0]*temp.X + gradient4[gi7][1]*temp.Y + gradient4[gi7][2]*temp.Z + gradient4[gi7][3]*temp.W;

    //Z inférieur, W superieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    temp.Z = nz-z0;
    temp.W = nw-(w0+1);
    s[2] = gradient4[gi8][0]*temp.X + gradient4[gi8][1]*temp.Y + gradient4[gi8][2]*temp.Z + gradient4[gi8][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[2] = gradient4[gi9][0]*temp.X + gradient4[gi9][1]*temp.Y + gradient4[gi9][2]*temp.Z + gradient4[gi9][3]*temp.W;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[2] = gradient4[gi10][0]*temp.X + gradient4[gi10][1]*temp.Y + gradient4[gi10][2]*temp.Z + gradient4[gi10][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[2] = gradient4[gi11][0]*temp.X + gradient4[gi11][1]*temp.Y + gradient4[gi11][2]*temp.Z + gradient4[gi11][3]*temp.W;

    //Z supérieur, W superieur
    temp.X = nx-x0;
    temp.Y = ny-y0;
    temp.Z = nz-(z0+1);
    s[3] = gradient4[gi12][0]*temp.X + gradient4[gi12][1]*temp.Y + gradient4[gi12][2]*temp.Z + gradient4[gi12][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-y0;
    t[3] = gradient4[gi13][0]*temp.X + gradient4[gi13][1]*temp.Y + gradient4[gi13][2]*temp.Z + gradient4[gi13][3]*temp.W;

    temp.X = nx-x0;
    temp.Y = ny-(y0+1);
    u[3] = gradient4[gi14][0]*temp.X + gradient4[gi14][1]*temp.Y + gradient4[gi14][2]*temp.Z + gradient4[gi14][3]*temp.W;

    temp.X = nx-(x0+1);
    temp.Y = ny-(y0+1);
    v[3] = gradient4[gi15][0]*temp.X + gradient4[gi15][1]*temp.Y + gradient4[gi15][2]*temp.Z + gradient4[gi15][3]*temp.W;

    //Li1 à Li8 : Lissage sur x
    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li1 = s[0] + Cx*(t[0]-s[0]);
    Li2 = u[0] + Cx*(v[0]-u[0]);
    Li3 = s[1] + Cx*(t[1]-s[1]);
    Li4 = u[1] + Cx*(v[1]-u[1]);
    Li5 = s[2] + Cx*(t[2]-s[2]);
    Li6 = u[2] + Cx*(v[2]-u[2]);
    Li7 = s[3] + Cx*(t[3]-s[3]);
    Li8 = u[3] + Cx*(v[3]-u[3]);

    //Li9 à Li12 : Lissage sur y
    tmp = ny-y0;
    Cy = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li9 = Li1 + Cy*(Li2-Li1);
    Li10 = Li3 + Cy*(Li4-Li3);
    Li11 = Li5 + Cy*(Li6-Li5);
    Li12 = Li7 + Cy*(Li8-Li7);

    //Li13,Li14 : Lissage sur z
    tmp = nz-z0;
    Cz = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li13 = Li9 + Cz*(Li10-Li9);
    Li14 = Li11 + Cz*(Li12-Li11);

    tmp = nw-w0;
    Cw = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return Li13 + Cw*(Li14-Li13);
}

//------------------------------ SIMPLEX ------------------------------

float NzNoiseMachine::Get2DSimplexNoiseValue(float x, float y, float resolution)
{
    x /= resolution;
    y /= resolution;


    Origin.X = fastfloor(x + (x+y)*SkewCoeff2D);
    Origin.Y = fastfloor(y + (x+y)*SkewCoeff2D);

    A.X = Origin.X - (Origin.X+Origin.Y)*UnskewCoeff2D;
    A.Y = Origin.Y - (Origin.X+Origin.Y)*UnskewCoeff2D;

    IsoOriginDist.X = x - A.X;
    IsoOriginDist.Y = y - A.Y;

    if(IsoOriginDist.X > IsoOriginDist.Y)
    {
        off1.X = 1;
        off1.Y = 0;
    }
    else
    {
        off1.X = 0;
        off1.Y = 1;
    }
    //On calcule les distances
    d1.X = A.X - x;
    d1.Y = A.Y - y;

    //?? Ordre des + et -
    d2.X = d1.X + off1.X - UnskewCoeff2D;
    d2.Y = d1.Y + off1.Y - UnskewCoeff2D;

    d3.X = d1.X + 1.0 - 2*UnskewCoeff2D;
    d3.Y = d1.Y + 1.0 - 2*UnskewCoeff2D;

    //On récupère la valeur des gradients des trois sommets
    ii = Origin.X & 255;
    jj = Origin.Y & 255;

    gi0 = perm[ii + perm[jj]] % 12;
    gi1 = perm[ii + off1.X + perm[jj + off1.Y]] % 12;
    gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

    H[0].X = gradient3[gi0][0];
    H[0].Y = gradient3[gi0][1];

    H[1].X = gradient3[gi1][0];
    H[1].Y = gradient3[gi1][1];

    H[2].X = gradient3[gi2][0];
    H[2].Y = gradient3[gi2][1];

    //On ajoute les valeurs ensemble


    n1 = H[0].X*d1.X + H[0].Y*d1.Y;
    n2 = H[1].X*d2.X + H[1].Y*d2.Y;
    n3 = H[2].X*d3.X + H[2].Y*d3.Y;

    lenght = 0.5;

    c1 = lenght - d1.X*d1.X - d1.Y*d1.Y;
    c2 = lenght - d2.X*d2.X - d2.Y*d2.Y;
    c3 = lenght - d3.X*d3.X - d3.Y*d3.Y;

    if(c1 < 0)
        c1 = 0;

    if(c2 < 0)
        c2 = 0;

    if(c3 < 0)
        c3 = 0;

    n1 = c1*c1*c1*n1;
    n2 = c2*c2*c2*n2;
    n3 = c3*c3*c3*n3;

    return (n1+n2+n3)*23.2;
}

float NzNoiseMachine::Get3DSimplexNoiseValue(float x, float y, float z, float resolution)
{
    x /= resolution;
    y /= resolution;
    z /= resolution;


    Origin.X = fastfloor(x + (x+y+z)*SkewCoeff3D);
    Origin.Y = fastfloor(y + (x+y+z)*SkewCoeff3D);
    Origin.Z = fastfloor(z + (x+y+z)*SkewCoeff3D);

    A.X = Origin.X - (Origin.X+Origin.Y+Origin.Z)*UnskewCoeff3D;
    A.Y = Origin.Y - (Origin.X+Origin.Y+Origin.Z)*UnskewCoeff3D;
    A.Z = Origin.Z - (Origin.X+Origin.Y+Origin.Z)*UnskewCoeff3D;

    IsoOriginDist.X = x - A.X;
    IsoOriginDist.Y = y - A.Y;
    IsoOriginDist.Z = z - A.Z;

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.

    if(IsoOriginDist.X >= IsoOriginDist.Y)
    {
        if(IsoOriginDist.Y >= IsoOriginDist.Z)
        {
            off1.X=1;
            off1.Y=0;
            off1.Z=0;
            off2.X=1;
            off2.Y=1;
            off2.Z=0;
        } // X Y Z order
        else if(IsoOriginDist.X >= IsoOriginDist.Z)
        {
            off1.X=1;    // X Z Y order
            off1.Y=0;
            off1.Z=0;
            off2.X=1;
            off2.Y=0;
            off2.Z=1;
        }
        else
        {
            off1.X=0;    // Z X Y order
            off1.Y=0;
            off1.Z=1;
            off2.X=1;
            off2.Y=0;
            off2.Z=1;
        }
    }
    else   // x0<y0
    {
        if(IsoOriginDist.Y < IsoOriginDist.Z)
        {
            off1.X=0;    // Z Y X order
            off1.Y=0;
            off1.Z=1;
            off2.X=0;
            off2.Y=1;
            off2.Z=1;
        }
        else if(IsoOriginDist.X < IsoOriginDist.Z)
        {
            off1.X=0;    // Y Z X order
            off1.Y=1;
            off1.Z=0;
            off2.X=0;
            off2.Y=1;
            off2.Z=1;
        }
        else
        {
            off1.X=0;    // Y X Z order
            off1.Y=1;
            off1.Z=0;
            off2.X=1;
            off2.Y=1;
            off2.Z=0;
        }
    }
    //On calcule les distances
    d1.X = A.X - x;
    d1.Y = A.Y - y;
    d1.Z = A.Z - z;

    d2.X = d1.X + off1.X - UnskewCoeff3D;
    d2.Y = d1.Y + off1.Y - UnskewCoeff3D;
    d2.Z = d1.Z + off1.Z - UnskewCoeff3D;

    d3.X = d1.X + off2.X - 2*UnskewCoeff3D;
    d3.Y = d1.Y + off2.Y - 2*UnskewCoeff3D;
    d3.Z = d1.Z + off2.Z - 2*UnskewCoeff3D;

    d4.X = d1.X + 1.0 - 3*UnskewCoeff3D;
    d4.Y = d1.Y + 1.0 - 3*UnskewCoeff3D;
    d4.Z = d1.Z + 1.0 - 3*UnskewCoeff3D;

    //On récupère la valeur des gradients des trois sommets
    ii = Origin.X & 255;
    jj = Origin.Y & 255;
    kk = Origin.Z & 255;

    gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
    gi1 = perm[ii + off1.X + perm[jj + off1.Y + perm[kk + off1.Z]]] % 12;
    gi2 = perm[ii + off2.X + perm[jj + off2.Y + perm[kk + off2.Z]]] % 12;
    gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12;

    //On ajoute les valeurs ensemble

    n1 = gradient3[gi0][0]*d1.X + gradient3[gi0][1]*d1.Y + gradient3[gi0][2]*d1.Z;
    n2 = gradient3[gi1][0]*d2.X + gradient3[gi1][1]*d2.Y + gradient3[gi1][2]*d2.Z;
    n3 = gradient3[gi2][0]*d3.X + gradient3[gi2][1]*d3.Y + gradient3[gi2][2]*d3.Z;
    n4 = gradient3[gi3][0]*d4.X + gradient3[gi3][1]*d4.Y + gradient3[gi3][2]*d4.Z;

    lenght = 0.6;

    c1 = lenght - d1.X*d1.X - d1.Y*d1.Y - d1.Z*d1.Z;
    c2 = lenght - d2.X*d2.X - d2.Y*d2.Y - d2.Z*d2.Z;
    c3 = lenght - d3.X*d3.X - d3.Y*d3.Y - d3.Z*d3.Z;
    c4 = lenght - d4.X*d4.X - d4.Y*d4.Y - d4.Z*d4.Z;

    if(c1 < 0)
        c1 = 0;

    if(c2 < 0)
        c2 = 0;

    if(c3 < 0)
        c3 = 0;

    if(c4 < 0)
        c4 = 0;

    n1 = c1*c1*c1*n1;
    n2 = c2*c2*c2*n2;
    n3 = c3*c3*c3*n3;
    n4 = c4*c4*c4*n4;

    return (n1+n2+n3+n4)*17.6995;
}

float NzNoiseMachine::Get4DSimplexNoiseValue(float x, float y, float z, float w, float resolution)
{
    x /= resolution;
    y /= resolution;
    z /= resolution;
    w /= resolution;

    Origin.X = fastfloor(x + (x+y+z+w)*SkewCoeff4D);
    Origin.Y = fastfloor(y + (x+y+z+w)*SkewCoeff4D);
    Origin.Z = fastfloor(z + (x+y+z+w)*SkewCoeff4D);
    Origin.W = fastfloor(w + (x+y+z+w)*SkewCoeff4D);

    A.X = Origin.X - (Origin.X+Origin.Y+Origin.Z+Origin.W)*UnskewCoeff4D;
    A.Y = Origin.Y - (Origin.X+Origin.Y+Origin.Z+Origin.W)*UnskewCoeff4D;
    A.Z = Origin.Z - (Origin.X+Origin.Y+Origin.Z+Origin.W)*UnskewCoeff4D;
    A.W = Origin.W - (Origin.X+Origin.Y+Origin.Z+Origin.W)*UnskewCoeff4D;

    IsoOriginDist.X = x - A.X;
    IsoOriginDist.Y = y - A.Y;
    IsoOriginDist.Z = z - A.Z;
    IsoOriginDist.W = w - A.W;

    // For the 4D case, the simplex is a 4D shape I won't even try to describe.
    // To find out which of the 24 possible simplices we're in, we need to
    // determine the magnitude ordering of x0, y0, z0 and w0.
    // The method below is a good way of finding the ordering of x,y,z,w and
    // then find the correct traversal order for the simplex we’re in.
    // First, six pair-wise comparisons are performed between each possible pair
    // of the four coordinates, and the results are used to add up binary bits
    // for an floateger index.
    c1 = (IsoOriginDist.X > IsoOriginDist.Y) ? 32 : 0;
    c2 = (IsoOriginDist.X > IsoOriginDist.Z) ? 16 : 0;
    c3 = (IsoOriginDist.Y > IsoOriginDist.Z) ? 8  : 0;
    c4 = (IsoOriginDist.X > IsoOriginDist.W) ? 4  : 0;
    c5 = (IsoOriginDist.Y > IsoOriginDist.W) ? 2  : 0;
    c6 = (IsoOriginDist.Z > IsoOriginDist.W) ? 1  : 0;
    c = c1 + c2 + c3 + c4 + c5 + c6;
    // simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
    // Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
    // impossible. Only the 24 indices which have non-zero entries make any sense.
    // We use a thresholding to set the coordinates in turn from the largest magnitude.
    // The number 3 in the "simplex" array is at the position of the largest coordinate.
    off1.X = lookupTable4D[c][0]>=3 ? 1 : 0;
    off1.Y = lookupTable4D[c][1]>=3 ? 1 : 0;
    off1.Z = lookupTable4D[c][2]>=3 ? 1 : 0;
    off1.W = lookupTable4D[c][3]>=3 ? 1 : 0;
    // The number 2 in the "simplex" array is at the second largest coordinate.
    off2.X = lookupTable4D[c][0]>=2 ? 1 : 0;
    off2.Y = lookupTable4D[c][1]>=2 ? 1 : 0;
    off2.Z = lookupTable4D[c][2]>=2 ? 1 : 0;
    off2.W = lookupTable4D[c][3]>=2 ? 1 : 0;
    // The number 1 in the "simplex" array is at the second smallest coordinate.
    off3.X = lookupTable4D[c][0]>=1 ? 1 : 0;
    off3.Y = lookupTable4D[c][1]>=1 ? 1 : 0;
    off3.Z = lookupTable4D[c][2]>=1 ? 1 : 0;
    off3.W = lookupTable4D[c][3]>=1 ? 1 : 0;
    // The fifth corner has all coordinate offsets = 1, so no need to look that up.

    //On calcule les distances
    d1.X = A.X - x;
    d1.Y = A.Y - y;
    d1.Z = A.Z - z;
    d1.W = A.W - w;

    d2.X = d1.X + off1.X - UnskewCoeff4D;
    d2.Y = d1.Y + off1.Y - UnskewCoeff4D;
    d2.Z = d1.Z + off1.Z - UnskewCoeff4D;
    d2.W = d1.W + off1.W - UnskewCoeff4D;

    d3.X = d1.X + off2.X - 2*UnskewCoeff4D;
    d3.Y = d1.Y + off2.Y - 2*UnskewCoeff4D;
    d3.Z = d1.Z + off2.Z - 2*UnskewCoeff4D;
    d3.W = d1.W + off2.W - 2*UnskewCoeff4D;

    d4.X = d1.X + off3.X - 3*UnskewCoeff4D;
    d4.Y = d1.Y + off3.Y - 3*UnskewCoeff4D;
    d4.Z = d1.Z + off3.Z - 3*UnskewCoeff4D;
    d4.W = d1.W + off3.W - 3*UnskewCoeff4D;

    d5.X = d1.X + 1.0 - 4*UnskewCoeff4D;
    d5.Y = d1.Y + 1.0 - 4*UnskewCoeff4D;
    d5.Z = d1.Z + 1.0 - 4*UnskewCoeff4D;
    d5.W = d1.W + 1.0 - 4*UnskewCoeff4D;

    //On récupère la valeur des gradients des trois sommets
    ii = Origin.X & 255;
    jj = Origin.Y & 255;
    kk = Origin.Z & 255;
    ll = Origin.W & 255;

    gi0 = perm[ii + perm[jj + perm[kk+perm[ll]]]] % 32;
    gi1 = perm[ii + off1.X + perm[jj + off1.Y + perm[kk + off1.Z + perm[ll + off1.W]]]] % 32;
    gi2 = perm[ii + off2.X + perm[jj + off2.Y + perm[kk + off2.Z + perm[ll + off2.W]]]] % 32;
    gi3 = perm[ii + off3.X + perm[jj + off3.Y + perm[kk + off3.Z + perm[ll + off3.W]]]] % 32;
    gi4 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] % 32;

    //On ajoute les valeurs ensemble

    n1 = gradient4[gi0][0]*d1.X + gradient4[gi0][1]*d1.Y + gradient4[gi0][2]*d1.Z + gradient4[gi0][3]*d1.W;
    n2 = gradient4[gi1][0]*d2.X + gradient4[gi1][1]*d2.Y + gradient4[gi1][2]*d2.Z + gradient4[gi1][3]*d2.W;
    n3 = gradient4[gi2][0]*d3.X + gradient4[gi2][1]*d3.Y + gradient4[gi2][2]*d3.Z + gradient4[gi2][3]*d3.W;
    n4 = gradient4[gi3][0]*d4.X + gradient4[gi3][1]*d4.Y + gradient4[gi3][2]*d4.Z + gradient4[gi3][3]*d4.W;
    n5 = gradient4[gi4][0]*d5.X + gradient4[gi4][1]*d5.Y + gradient4[gi4][2]*d5.Z + gradient4[gi4][3]*d5.W;

    lenght = 0.6;

    c1 = lenght - d1.X*d1.X - d1.Y*d1.Y - d1.Z*d1.Z - d1.W*d1.W;
    c2 = lenght - d2.X*d2.X - d2.Y*d2.Y - d2.Z*d2.Z - d2.W*d2.W;
    c3 = lenght - d3.X*d3.X - d3.Y*d3.Y - d3.Z*d3.Z - d3.W*d3.W;
    c4 = lenght - d4.X*d4.X - d4.Y*d4.Y - d4.Z*d4.Z - d4.W*d4.W;
    c5 = lenght - d5.X*d5.X - d5.Y*d5.Y - d5.Z*d5.Z - d5.W*d5.W;

    if(c1 < 0)
        c1 = 0;

    if(c2 < 0)
        c2 = 0;

    if(c3 < 0)
        c3 = 0;

    if(c4 < 0)
        c4 = 0;

    if(c5 < 0)
        c5 = 0;

    n1 = c1*c1*c1*n1;
    n2 = c2*c2*c2*n2;
    n3 = c3*c3*c3*n3;
    n4 = c4*c4*c4*n4;
    n5 = c5*c5*c5*n5;

    //return n3*30;
    return (n1+n2+n3+n4+n5)*17.6995;
}

//------------------------------ CELL ------------------------------

float NzNoiseMachine::Get2DCellNoiseValue(float x, float y, float res)
{

}

float NzNoiseMachine::Get3DCellNoiseValue(float x, float y, float z, float res)
{
    x /= res;
    y /= res;
    z /= res;

    //On récupère les positions de la grille associée à (x,y)
    x0 = (float)(x);
    y0 = (float)(y);
    z0 = (float)(z);

    return 0;//(float)((float)(this->Random3D(x0,y0,z0,SEED_A)) % 1000)/1000;
}
float NzNoiseMachine::Get4DCellNoiseValue(float x, float y, float z, float w, float res)
{
    x /= res;
    y /= res;
    z /= res;
    w /= res;

    //On récupère les positions de la grille associée à (x,y)
    x0 = (float)(x) & 255;
    y0 = (float)(y) & 255;
    z0 = (float)(z) & 255;
    w0 = (float)(w) & 255;

    return perm[x0 + perm[y0 + perm[z0 + perm[w0]]]] / 128.f - 1;
}

void NzNoiseMachine::SetLacunarity(float lacunarity)
{
    if(lacunarity != m_lacunarity)
    {
        m_lacunarity = lacunarity;
        m_parametersModified = true;
    }
}

void NzNoiseMachine::SetHurstParameter(float h)
{
    if(h != m_hurst)
    {
        m_hurst = h;
        m_parametersModified = true;
    }
}

void NzNoiseMachine::SetOctavesNumber(float octaves)
{
    if(octaves != m_octaves && octaves < 30)
    {
        m_octaves = h;
        m_parametersModified = true;
    }
}

void NzNoiseMachine::RecomputeExponentArray()
{
    // precompute and store spectral weights
    float frequency = 1.0;
    for (float i(0) ; i < m_octaves; ++i)
    {
        //compute weight for each frequency
        exponent_array[i] = pow( frequency, -m_hurst );
        frequency *= m_lacunarity;

    }
    m_parametersModified = false;
}

//------------------------------ FBM ------------------------------

float NzNoiseMachine::Get1DFBMNoiseValue(float x, float res)
{
    value = 0.0;

    if(parametersModified)
    {
        RecomputeExponentArray();
    }

    // inner loop of spectral construction
    for (float i(0); i < octaves; ++i)
    {
        value += Get1DPerlinNoiseValue(x,resolution) * exponent_array[i];
        x *= lacunarity;
    }
    remainder = octaves - (float)octaves;

    //Doesn't seem to work
    if(remainder != 0)
        value += remainder * Get1DPerlinNoiseValue(x,resolution) * exponent_array[(float)octaves-1];

    return value*sum;
}

float NzNoiseMachine::Get2DFBMNoiseValue(float x, float y, float res)
{
    value = 0.0;

    if(parametersModified)
    {
        RecomputeExponentArray();
    }

    // inner loop of spectral construction
    for (float i(0); i < octaves; ++i)
    {
        value += Get2DdPerlinNoiseValue(x,y,resolution) * exponent_array[i];
        x *= lacunarity;
        y *= lacunarity;
    }
    remainder = octaves - (float)octaves;

    //Doesn't seem to work
    if(remainder != 0)
        value += remainder * Get2DSimplexNoiseValue(x,y,resolution) * exponent_array[(float)octaves-1];

    return value*sum;

}
float NzNoiseMachine::Get3DFBMNoiseValue(float x, float y, float z, float res)
{
    value = 0.0;

    if(parametersModified)
    {
        RecomputeExponentArray();
    }

    // inner loop of spectral construction
    for (float i(0); i < octaves; ++i)
    {
        value += Get3DSimplexNoiseValue(x,y,z,resolution) * exponent_array[i];
        x *= lacunarity;
        y *= lacunarity;
        z *= lacunarity;
    }
    remainder = octaves - (float)octaves;

    //Doesn't seem to work
    if(remainder != 0)
        value += remainder * Get3DSimplexNoiseValue(x,y,z,resolution) * exponent_array[(float)octaves-1];

    return value*sum;
}

//------------------------------ HYBRID MULTIFRACTAL ------------------------------

float NzNoiseMachine::Get2DHybridMultiFractalNoiseValue(float x, float y, float res)
{
    float frequency, result, signal, weight, remainder;
    float offset = 1;

    if(parametersModified)
    {
        RecomputeExponentArray();
    }

    // get first octave
    result = (Get2DSimplexNoiseValue(x,y,resolution) + offset) * exponent_array[0];
    weight = result;
    // increase frequency
    x *= lacunarity;
    y *= lacunarity;

    // spectral construction inner loop
    for (float i(1) ; i < octaves; ++i)
    {
        // prevent divergence
        if ( weight > 1.0 )
            weight = 1.0;

        signal = ( Get2DSimplexNoiseValue(x,y,resolution) + offset ) * exponent_array[i];
        result += weight * signal;
        // update the (monotonically decreasing) weighting value
        // (this is why H must specify a high fractal dimension)
        weight *= signal;

        x *= lacunarity;
        y *= lacunarity;

    }
    // take care of remainder
    remainder = octaves - (float)octaves;

    if(remainder != 0)
        result += remainder * Get2DSimplexNoiseValue(x,y,resolution) * exponent_array[(float)octaves-1];

    return result;

}

float NzNoiseMachine::Get3DHybridMultiFractalNoiseValue(float x, float y, float z, float res)
{
    float frequency, result, signal, weight, remainder;
    float offset = 1;

    if(parametersModified)
    {
        RecomputeExponentArray();
    }
    // get first octave of function
    result = (Get3DSimplexNoiseValue(x,y,z,resolution) + offset) * exponent_array[0];
    weight = result;
    // increase frequency
    x *= lacunarity;
    y *= lacunarity;

    // spectral construction inner loop
    for (float i(1) ; i < octaves; ++i)
    {
        // prevent divergence
        if ( weight > 1.0 )
            weight = 1.0;

        signal = ( Get3DSimplexNoiseValue(x,y,z,resolution) + offset ) * exponent_array[i];
        result += weight * signal;
        // update the (monotonically decreasing) weighting value
        // (this is why H must specify a high fractal dimension)
        weight *= signal;

        x *= lacunarity;
        y *= lacunarity;

    }
    // take care of remainder
    remainder = octaves - (float)octaves;

    if(remainder != 0)
        result += remainder * Get3DSimplexNoiseValue(x,y,z,resolution) * exponent_array[(float)octaves-1];

    return result;
}


float NzNoiseMachine::fastfloor(float n)
{
    if(n >= 0)
        return float(n);
    else
        return float(n-1);
}
