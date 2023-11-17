xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 63;
 -1.02403;-11.80193;1.57338;,
 -0.00124;-11.79714;2.01625;,
 -0.00124;-10.25205;2.09991;,
 -1.43749;-10.25762;1.67199;,
 1.02155;-11.80193;1.57338;,
 1.43501;-10.25762;1.67199;,
 1.44878;-11.81350;0.50421;,
 1.92194;-10.27081;0.45342;,
 1.02155;-11.82507;-0.56496;,
 1.43501;-10.28399;-0.76515;,
 -0.00124;-11.82986;-1.00782;,
 -0.00124;-10.28945;-1.26990;,
 -1.02403;-11.82507;-0.56496;,
 -1.43749;-10.28399;-0.76515;,
 -1.45126;-11.81350;0.50421;,
 -1.92441;-10.27081;0.45342;,
 -1.02403;-11.80193;1.57338;,
 -1.43749;-10.25762;1.67199;,
 -0.00124;-6.65450;3.18257;,
 -1.72229;-6.69631;2.08686;,
 1.71981;-6.69631;2.08686;,
 2.35433;-6.71349;0.49891;,
 1.71981;-6.73067;-1.08903;,
 -0.00124;-6.73779;-1.74678;,
 -1.72229;-6.73067;-1.08903;,
 -2.35681;-6.71349;0.49891;,
 -1.72229;-6.69631;2.08686;,
 -0.00124;-3.49537;3.40596;,
 -1.93991;-3.50323;2.30195;,
 1.93743;-3.50323;2.30195;,
 2.68341;-3.52342;0.43508;,
 1.93743;-3.54362;-1.43179;,
 -0.00124;-3.55198;-2.20508;,
 -1.93991;-3.54362;-1.43179;,
 -2.68589;-3.52342;0.43508;,
 -1.93991;-3.50323;2.30195;,
 -0.00124;0.02826;3.08348;,
 -1.81796;0.01754;2.61194;,
 1.75570;0.01754;2.61194;,
 2.98749;0.01709;0.37428;,
 2.03227;-0.03087;-1.86339;,
 -0.00124;-0.04090;-2.79027;,
 -2.03474;-0.03087;-1.86339;,
 -2.98997;0.01709;0.37428;,
 -1.81796;0.01754;2.61194;,
 -0.00124;-11.79714;2.01625;,
 -1.02403;-11.80193;1.57338;,
 -0.00124;-11.81350;0.50421;,
 1.02155;-11.80193;1.57338;,
 1.44878;-11.81350;0.50421;,
 1.02155;-11.82507;-0.56496;,
 -0.00124;-11.82986;-1.00782;,
 -1.02403;-11.82507;-0.56496;,
 -1.45126;-11.81350;0.50421;,
 -1.81796;0.01754;2.61194;,
 -0.00124;0.02826;3.08348;,
 -0.00124;2.44305;-0.51773;,
 1.75570;0.01754;2.61194;,
 2.98749;0.01709;0.37428;,
 2.03227;-0.03087;-1.86339;,
 -0.00124;-0.04090;-2.79027;,
 -2.03474;-0.03087;-1.86339;,
 -2.98997;0.01709;0.37428;;
 
 48;
 4;0,1,2,3;,
 4;1,4,5,2;,
 4;4,6,7,5;,
 4;6,8,9,7;,
 4;8,10,11,9;,
 4;10,12,13,11;,
 4;12,14,15,13;,
 4;14,16,17,15;,
 4;3,2,18,19;,
 4;2,5,20,18;,
 4;5,7,21,20;,
 4;7,9,22,21;,
 4;9,11,23,22;,
 4;11,13,24,23;,
 4;13,15,25,24;,
 4;15,17,26,25;,
 4;19,18,27,28;,
 4;18,20,29,27;,
 4;20,21,30,29;,
 4;21,22,31,30;,
 4;22,23,32,31;,
 4;23,24,33,32;,
 4;24,25,34,33;,
 4;25,26,35,34;,
 4;28,27,36,37;,
 4;27,29,38,36;,
 4;29,30,39,38;,
 4;30,31,40,39;,
 4;31,32,41,40;,
 4;32,33,42,41;,
 4;33,34,43,42;,
 4;34,35,44,43;,
 3;45,46,47;,
 3;48,45,47;,
 3;49,48,47;,
 3;50,49,47;,
 3;51,50,47;,
 3;52,51,47;,
 3;53,52,47;,
 3;46,53,47;,
 3;54,55,56;,
 3;55,57,56;,
 3;57,58,56;,
 3;58,59,56;,
 3;59,60,56;,
 3;60,61,56;,
 3;61,62,56;,
 3;62,54,56;;
 
 MeshMaterialList {
  1;
  48;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\neet3.jpg";
   }
  }
 }
 MeshNormals {
  54;
  -0.676739;-0.194908;0.709955;,
  0.000000;-0.099108;0.995077;,
  0.676737;-0.194909;0.709956;,
  0.956072;-0.293105;0.003996;,
  0.679247;-0.252970;-0.688934;,
  0.000000;-0.193776;-0.981046;,
  -0.679248;-0.252968;-0.688933;,
  -0.956073;-0.293102;0.003996;,
  -0.695576;-0.180536;0.695400;,
  0.000000;-0.154007;0.988070;,
  0.695575;-0.180536;0.695401;,
  0.978144;-0.207913;0.002471;,
  0.687226;-0.189164;-0.701382;,
  0.000000;-0.160382;-0.987055;,
  -0.687228;-0.189164;-0.701381;,
  -0.978144;-0.207912;0.002470;,
  -0.737099;-0.126421;0.663854;,
  0.000000;-0.152155;0.988357;,
  0.737098;-0.126421;0.663855;,
  0.993748;-0.111637;0.001078;,
  0.696028;-0.124891;-0.707069;,
  -0.000000;-0.132217;-0.991221;,
  -0.696029;-0.124891;-0.707069;,
  -0.993748;-0.111637;0.001078;,
  -0.726283;-0.051293;0.685480;,
  -0.185150;-0.015906;0.982581;,
  0.725299;-0.048415;0.686729;,
  0.996473;-0.082792;0.013643;,
  0.703853;-0.121090;-0.699949;,
  -0.000000;-0.142836;-0.989746;,
  -0.703853;-0.121090;-0.699949;,
  -0.996368;-0.084435;0.011051;,
  -0.713874;0.294499;0.635338;,
  0.002578;0.493628;0.869669;,
  0.708638;0.296729;0.640144;,
  0.968952;0.227781;-0.096166;,
  0.667410;0.328257;-0.668439;,
  -0.000001;0.298528;-0.954401;,
  -0.667410;0.328257;-0.668440;,
  -0.912427;0.409123;0.009780;,
  -0.000000;-0.999942;0.010820;,
  0.155754;0.930213;0.332331;,
  0.513633;-0.077191;0.854531;,
  0.000000;-0.999941;0.010819;,
  -0.000001;-0.999941;0.010821;,
  0.000001;-0.999941;0.010821;,
  0.000000;-0.999941;0.010821;,
  -0.000001;-0.999941;0.010820;,
  -0.000000;-0.999941;0.010819;,
  0.000001;-0.999941;0.010820;,
  -0.147814;0.821435;0.550813;,
  0.152728;0.820815;0.550397;,
  0.545555;0.782479;0.300161;,
  -0.243457;0.736689;-0.630886;;
  48;
  4;0,1,9,8;,
  4;1,2,10,9;,
  4;2,3,11,10;,
  4;3,4,12,11;,
  4;4,5,13,12;,
  4;5,6,14,13;,
  4;6,7,15,14;,
  4;7,0,8,15;,
  4;8,9,17,16;,
  4;9,10,18,17;,
  4;10,11,19,18;,
  4;11,12,20,19;,
  4;12,13,21,20;,
  4;13,14,22,21;,
  4;14,15,23,22;,
  4;15,8,16,23;,
  4;16,17,25,24;,
  4;17,18,26,42;,
  4;18,19,27,26;,
  4;19,20,28,27;,
  4;20,21,29,28;,
  4;21,22,30,29;,
  4;22,23,31,30;,
  4;23,16,24,31;,
  4;24,25,33,32;,
  4;25,26,34,33;,
  4;26,27,35,34;,
  4;27,28,36,35;,
  4;28,29,37,36;,
  4;29,30,38,37;,
  4;30,31,39,38;,
  4;31,24,32,39;,
  3;43,44,40;,
  3;45,43,40;,
  3;46,45,40;,
  3;47,46,40;,
  3;48,47,40;,
  3;49,48,40;,
  3;46,49,40;,
  3;44,46,40;,
  3;50,33,41;,
  3;33,51,41;,
  3;34,52,41;,
  3;35,36,41;,
  3;36,37,53;,
  3;37,38,53;,
  3;38,39,53;,
  3;39,32,41;;
 }
 MeshTextureCoords {
  63;
  0.241307;0.097454;,
  0.244918;0.097454;,
  0.244918;0.086591;,
  0.241307;0.086591;,
  0.248530;0.097454;,
  0.248530;0.086591;,
  0.252142;0.097454;,
  0.252142;0.086591;,
  0.255754;0.097454;,
  0.255754;0.086591;,
  0.259366;0.097454;,
  0.259366;0.086591;,
  0.262977;0.097454;,
  0.262977;0.086591;,
  0.266589;0.097454;,
  0.266589;0.086591;,
  0.270201;0.097454;,
  0.270201;0.086591;,
  0.244918;0.075729;,
  0.241307;0.075729;,
  0.248530;0.075729;,
  0.252142;0.075729;,
  0.255754;0.075729;,
  0.259366;0.075729;,
  0.262977;0.075729;,
  0.266589;0.075729;,
  0.270201;0.075729;,
  0.244918;0.064866;,
  0.241307;0.064866;,
  0.248530;0.064866;,
  0.252142;0.064866;,
  0.255754;0.064866;,
  0.259366;0.064866;,
  0.262977;0.064866;,
  0.266589;0.064866;,
  0.270201;0.064866;,
  0.244918;0.054003;,
  0.241307;0.054003;,
  0.248530;0.054003;,
  0.252142;0.054003;,
  0.255754;0.054003;,
  0.259366;0.054003;,
  0.262977;0.054003;,
  0.266589;0.054003;,
  0.270201;0.054003;,
  0.255754;0.133572;,
  0.268523;0.128282;,
  0.255754;0.116235;,
  0.242984;0.128282;,
  0.237695;0.115513;,
  0.242984;0.102743;,
  0.255754;0.097454;,
  0.268523;0.102743;,
  0.273813;0.115513;,
  0.268523;0.023283;,
  0.255754;0.017994;,
  0.255754;0.036775;,
  0.242984;0.023283;,
  0.237695;0.036053;,
  0.242984;0.048823;,
  0.255754;0.054112;,
  0.268523;0.048823;,
  0.273813;0.036053;;
 }
}