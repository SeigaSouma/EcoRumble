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
 155;
 -10.10631;3.60986;0.12318;,
 -9.97453;3.59086;0.60315;,
 -8.42312;3.13855;0.15930;,
 -8.55490;3.15754;-0.32067;,
 -8.59862;3.00186;-0.31483;,
 -8.46683;2.98287;0.16515;,
 -10.01824;3.43519;0.60900;,
 -10.15003;3.45417;0.12902;,
 -8.47777;2.62520;-0.20071;,
 -12.45745;2.79699;-0.29894;,
 -12.45745;3.14050;-0.46564;,
 -8.47777;2.96872;-0.36740;,
 -12.45745;3.30404;-0.12863;,
 -8.47777;3.13226;-0.03040;,
 -12.45745;2.96053;0.03806;,
 -8.47777;2.78875;0.13630;,
 -16.70697;2.83897;-0.83300;,
 -16.70697;3.00251;-0.49599;,
 -16.70697;3.34602;-0.66269;,
 -16.70697;3.18248;-0.99970;,
 -8.48539;2.95370;0.36280;,
 -12.46743;3.13340;0.43927;,
 -12.50770;2.79430;0.26847;,
 -8.52566;2.61459;0.19200;,
 -12.49941;2.96198;-0.06640;,
 -8.51738;2.78228;-0.14286;,
 -12.45915;3.30108;0.10441;,
 -8.47711;3.12138;0.02794;,
 -16.72008;3.22053;0.89468;,
 -16.71180;3.38821;0.55982;,
 -16.75207;3.04910;0.38902;,
 -16.76035;2.88142;0.72388;,
 -8.47109;-0.72510;-0.58764;,
 -8.47109;-1.02545;0.00000;,
 -2.45997;-1.32579;0.00000;,
 -2.45997;-0.93747;-1.00005;,
 -8.47109;0.00000;-0.82985;,
 -2.45997;0.00000;-1.31320;,
 -8.47109;0.72511;-0.58764;,
 -2.45997;0.93749;-1.00005;,
 -8.47109;1.02545;0.00000;,
 -2.45997;1.32580;0.00000;,
 0.00000;0.00000;0.00000;,
 0.93177;0.00000;0.00000;,
 0.53957;-1.44388;-0.00000;,
 0.53957;-0.82382;-1.00365;,
 0.53957;0.00000;-1.36666;,
 0.53957;0.82383;-1.00365;,
 0.53957;1.44389;0.00000;,
 -2.45997;-1.96293;-1.82701;,
 -2.45997;-2.77600;0.00000;,
 -8.47109;-3.07635;0.00000;,
 -8.47109;-2.17530;-1.75713;,
 -2.45997;0.00000;-2.48270;,
 -8.47109;0.00000;-2.48376;,
 -2.45997;1.96294;-1.82701;,
 -8.47109;2.17531;-1.75713;,
 -2.45997;2.77601;0.00000;,
 -8.47109;3.07635;0.00000;,
 -12.45265;-2.29476;-2.27927;,
 -12.45265;-3.24528;0.00000;,
 -16.73444;-3.34748;0.00000;,
 -16.73444;-2.36703;-2.96487;,
 -12.45265;0.00000;-3.04581;,
 -16.73444;0.00000;-3.75554;,
 -12.45265;2.29477;-2.27927;,
 -16.73444;2.36704;-2.96487;,
 -12.45265;3.24530;-0.05676;,
 -16.73444;3.34750;-0.49705;,
 -16.61586;-2.66724;-3.25903;,
 -16.63114;-3.79983;0.00000;,
 -20.45365;-4.64305;0.00000;,
 -20.43837;-3.26348;-4.21418;,
 -16.60059;0.00000;-4.19881;,
 -20.42309;0.00000;-5.35312;,
 -16.61160;2.66316;-3.26793;,
 -20.43411;3.25941;-4.22307;,
 -16.62262;3.79168;-0.65990;,
 -20.44512;4.63490;-1.29509;,
 -20.55695;-2.96327;-3.92001;,
 -20.55695;-4.19070;0.00000;,
 -16.73444;-1.59873;0.00000;,
 -16.73444;-1.13047;-1.15883;,
 -20.55695;0.00000;-4.90985;,
 -16.73444;0.00000;-2.34529;,
 -20.55695;2.96328;-3.92001;,
 -16.73444;1.13048;-1.15883;,
 -20.55695;4.19070;-1.13224;,
 -16.73444;1.59874;-0.49705;,
 -16.82817;0.01408;-0.00233;,
 -12.51720;-0.46493;0.00000;,
 -12.54049;-0.19638;-0.28160;,
 -12.56376;0.00000;-0.80690;,
 -12.55318;0.22554;-0.22750;,
 -12.54259;0.52323;0.41548;,
 0.00000;-2.61206;-0.00000;,
 -12.45265;1.30384;-0.05676;,
 -12.45265;-1.30383;0.00000;,
 0.00000;2.61206;0.00000;,
 -12.45265;-0.92194;-0.80024;,
 -12.45265;0.00000;-1.48015;,
 -12.45265;0.92196;-0.80024;,
 -8.47109;-0.72510;0.58764;,
 -2.45997;-0.93747;1.00005;,
 -8.47109;0.00000;0.82985;,
 -2.45997;0.00000;1.31320;,
 -8.47109;0.72511;0.58764;,
 -2.45997;0.93749;1.00005;,
 0.53957;-0.82382;1.00365;,
 0.53957;0.00000;1.36666;,
 0.53957;0.82383;1.00365;,
 -2.45997;-1.96293;1.82701;,
 -8.47109;-2.17530;1.75713;,
 -2.45997;0.00000;2.48270;,
 -8.47109;0.00000;2.48376;,
 -2.45997;1.96294;1.82701;,
 -8.47109;2.17531;1.75713;,
 -12.45265;-2.29476;2.27927;,
 -16.73444;-2.36703;2.96487;,
 -12.45265;0.00000;3.04581;,
 -16.73444;0.00000;3.75554;,
 -12.45265;2.29477;2.27927;,
 -16.73444;2.36704;2.96487;,
 -12.45265;3.24530;0.05676;,
 -16.73444;3.34750;0.44243;,
 -16.61586;-2.66724;3.25904;,
 -20.43837;-3.26348;4.21418;,
 -16.60059;0.00000;4.19881;,
 -20.42309;0.00000;5.35312;,
 -16.61160;2.66316;3.26793;,
 -20.43411;3.25941;4.22307;,
 -16.62262;3.79168;0.60528;,
 -20.44512;4.63490;1.11860;,
 -20.55695;-2.96327;3.92001;,
 -16.73444;-1.13047;1.15883;,
 -20.55695;0.00000;4.90985;,
 -16.73444;0.00000;2.34529;,
 -20.55695;2.96328;3.92001;,
 -16.73444;1.13048;1.15883;,
 -20.55695;4.19070;0.95575;,
 -16.73444;1.59874;0.44243;,
 -12.54049;-0.19638;0.28160;,
 -12.56376;0.00000;0.80690;,
 -12.55135;0.22150;0.23397;,
 -12.53893;0.51514;-0.40253;,
 -12.45265;1.30384;0.05676;,
 -12.45265;-0.92194;0.80024;,
 -12.45265;0.00000;1.48015;,
 -12.45265;0.92196;0.80024;,
 0.00000;-1.49034;-1.81565;,
 0.00000;0.00000;-2.47235;,
 0.00000;1.49035;-1.81565;,
 0.00000;1.49035;1.81565;,
 0.00000;0.00000;2.47235;,
 0.00000;-1.49034;1.81565;;
 
 186;
 4;0,1,2,3;,
 4;4,3,2,5;,
 4;4,5,6,7;,
 4;7,6,1,0;,
 4;1,6,5,2;,
 4;7,0,3,4;,
 4;8,9,10,11;,
 4;11,10,12,13;,
 4;13,12,14,15;,
 4;15,14,9,8;,
 4;16,17,18,19;,
 4;15,8,11,13;,
 4;9,14,17,16;,
 4;14,12,18,17;,
 4;12,10,19,18;,
 4;10,9,16,19;,
 4;20,21,22,23;,
 4;23,22,24,25;,
 4;25,24,26,27;,
 4;27,26,21,20;,
 4;28,29,30,31;,
 4;27,20,23,25;,
 4;21,26,29,28;,
 4;26,24,30,29;,
 4;24,22,31,30;,
 4;22,21,28,31;,
 4;32,33,34,35;,
 4;36,32,35,37;,
 4;38,36,37,39;,
 4;40,38,39,41;,
 3;35,34,42;,
 3;37,35,42;,
 3;39,37,42;,
 3;41,39,42;,
 3;43,44,45;,
 3;43,45,46;,
 3;43,46,47;,
 3;43,47,48;,
 4;49,50,51,52;,
 4;53,49,52,54;,
 4;55,53,54,56;,
 4;57,55,56,58;,
 4;59,60,61,62;,
 4;63,59,62,64;,
 4;65,63,64,66;,
 4;67,65,66,68;,
 4;69,70,71,72;,
 4;73,69,72,74;,
 4;75,73,74,76;,
 4;77,75,76,78;,
 4;79,80,81,82;,
 4;83,79,82,84;,
 4;85,83,84,86;,
 4;87,85,86,88;,
 3;89,90,91;,
 3;89,91,92;,
 3;89,92,93;,
 3;89,93,94;,
 4;34,50,95,42;,
 4;96,67,68,88;,
 4;60,97,81,61;,
 3;61,81,80;,
 4;34,33,51,50;,
 4;41,57,58,40;,
 3;88,68,87;,
 4;41,42,98,57;,
 4;51,33,97,60;,
 4;99,97,33,32;,
 4;100,99,32,36;,
 4;101,100,36,38;,
 4;96,101,38,40;,
 4;40,58,67,96;,
 4;58,56,65,67;,
 4;56,54,63,65;,
 4;54,52,59,63;,
 4;52,51,60,59;,
 4;102,103,34,33;,
 4;104,105,103,102;,
 4;106,107,105,104;,
 4;40,41,107,106;,
 3;103,42,34;,
 3;105,42,103;,
 3;107,42,105;,
 3;41,42,107;,
 3;43,108,44;,
 3;43,109,108;,
 3;43,110,109;,
 3;43,48,110;,
 4;111,112,51,50;,
 4;113,114,112,111;,
 4;115,116,114,113;,
 4;57,58,116,115;,
 4;117,118,61,60;,
 4;119,120,118,117;,
 4;121,122,120,119;,
 4;123,124,122,121;,
 4;125,126,71,70;,
 4;127,128,126,125;,
 4;129,130,128,127;,
 4;131,132,130,129;,
 4;133,134,81,80;,
 4;135,136,134,133;,
 4;137,138,136,135;,
 4;139,140,138,137;,
 3;89,141,90;,
 3;89,142,141;,
 3;89,143,142;,
 3;89,144,143;,
 4;34,42,95,50;,
 4;145,140,124,123;,
 4;60,61,81,97;,
 3;61,80,81;,
 4;34,50,51,33;,
 4;41,40,58,57;,
 3;140,139,124;,
 4;41,57,98,42;,
 4;51,60,97,33;,
 4;146,102,33,97;,
 4;147,104,102,146;,
 4;148,106,104,147;,
 4;145,40,106,148;,
 4;40,145,123,58;,
 4;58,123,121,116;,
 4;116,121,119,114;,
 4;114,119,117,112;,
 4;112,117,60,51;,
 4;62,61,70,69;,
 4;61,80,71,70;,
 4;80,79,72,71;,
 4;64,62,69,73;,
 4;79,83,74,72;,
 4;66,64,73,75;,
 4;83,85,76,74;,
 4;68,66,75,77;,
 4;85,87,78,76;,
 4;87,68,77,78;,
 4;133,80,71,126;,
 4;80,61,70,71;,
 4;61,118,125,70;,
 4;135,133,126,128;,
 4;118,120,127,125;,
 4;137,135,128,130;,
 4;120,122,129,127;,
 4;124,139,132,131;,
 4;139,137,130,132;,
 4;122,124,131,129;,
 4;149,95,50,49;,
 4;150,149,49,53;,
 4;151,150,53,55;,
 4;98,151,55,57;,
 4;152,98,57,115;,
 4;153,152,115,113;,
 4;154,153,113,111;,
 4;95,154,111,50;,
 4;42,95,44,43;,
 4;95,149,45,44;,
 4;149,150,46,45;,
 4;150,151,47,46;,
 4;151,98,48,47;,
 4;98,42,43,48;,
 4;154,95,44,108;,
 4;95,42,43,44;,
 4;153,154,108,109;,
 4;152,153,109,110;,
 4;42,98,48,43;,
 4;98,152,110,48;,
 3;82,81,89;,
 4;81,97,90,89;,
 4;97,99,91,90;,
 3;84,82,89;,
 4;99,100,92,91;,
 3;86,84,89;,
 4;100,101,93,92;,
 3;88,86,89;,
 4;101,96,94,93;,
 4;96,88,89,94;,
 4;146,97,90,141;,
 4;97,81,89,90;,
 3;81,134,89;,
 4;147,146,141,142;,
 3;134,136,89;,
 4;148,147,142,143;,
 3;136,138,89;,
 4;140,145,144,89;,
 4;145,148,143,144;,
 3;138,140,89;;
 
 MeshMaterialList {
  2;
  186;
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
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
    "data\\TEXTURE\\tyuuni\\tyuuni_suit.png";
   }
  }
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\tyuuni\\tyuuni_metal.png";
   }
  }
 }
 MeshNormals {
  206;
  0.270181;0.962132;-0.036104;,
  0.925747;-0.269896;-0.264858;,
  -0.270175;-0.962133;0.036127;,
  0.003362;-0.436573;-0.899663;,
  0.055815;-0.435903;-0.898261;,
  0.049549;0.898558;-0.436048;,
  0.056589;0.898223;-0.435882;,
  -0.003363;0.436578;0.899660;,
  -0.055815;0.435900;0.898263;,
  -1.000000;0.000000;0.000000;,
  -0.003133;-0.449541;0.893254;,
  0.041226;-0.453358;0.890374;,
  -0.048889;-0.892603;-0.448185;,
  -0.057570;-0.892106;-0.448144;,
  0.003135;0.449557;-0.893246;,
  -0.041225;0.453371;-0.890368;,
  -0.994177;0.104170;0.027580;,
  -0.008001;0.999968;-0.000000;,
  -0.010190;0.693544;0.720342;,
  -0.012823;0.000003;0.999918;,
  -0.010923;-0.690861;0.722905;,
  -0.009575;-0.999954;0.000000;,
  -0.222352;0.974966;0.000000;,
  -0.219146;0.663788;0.715094;,
  -0.205188;0.000003;0.978723;,
  -0.219148;-0.663789;0.715093;,
  -0.222353;-0.974966;0.000000;,
  -0.217354;0.133877;0.966868;,
  0.083182;-0.996534;-0.000000;,
  0.049383;-0.662028;-0.747850;,
  0.018447;-0.000001;-0.999830;,
  0.049383;0.662028;-0.747850;,
  0.083183;0.996534;0.000000;,
  0.051882;-0.998653;0.000000;,
  0.061707;-0.663432;-0.745688;,
  0.070183;-0.000001;-0.997534;,
  0.062689;0.662532;-0.746405;,
  0.053997;0.998541;-0.000000;,
  0.047099;-0.998890;-0.000000;,
  0.110557;-0.692322;-0.713069;,
  0.163525;-0.000001;-0.986539;,
  0.122956;0.678831;-0.723927;,
  0.068436;0.922299;-0.380370;,
  -0.568998;0.822339;0.000000;,
  -0.597487;0.627584;0.499148;,
  -0.665285;0.000001;0.746590;,
  -0.598955;-0.578665;0.553534;,
  -0.569286;-0.714624;0.406480;,
  -0.903850;0.427849;-0.000008;,
  -0.910820;0.320787;0.259812;,
  -0.929075;0.000153;0.369891;,
  -0.909170;-0.299166;0.289672;,
  -0.899094;-0.382151;0.213519;,
  0.055313;-0.998469;-0.000000;,
  -0.000064;0.000237;1.000000;,
  -0.086840;0.711604;0.697193;,
  -0.111936;0.000004;0.993715;,
  -0.088093;-0.706380;0.702329;,
  -0.070521;0.141348;0.987445;,
  0.065042;0.913139;-0.402426;,
  0.114282;0.670487;-0.733067;,
  0.151664;-0.000001;-0.988432;,
  0.107147;-0.678133;-0.727087;,
  -0.010190;0.693544;-0.720342;,
  -0.012823;0.000003;-0.999918;,
  -0.010923;-0.690861;-0.722905;,
  -0.219146;0.663788;-0.715094;,
  -0.205188;0.000003;-0.978723;,
  -0.219148;-0.663789;-0.715093;,
  0.049383;-0.662028;0.747850;,
  0.018447;-0.000001;0.999830;,
  0.049383;0.662028;0.747850;,
  0.061707;-0.663432;0.745688;,
  0.070183;-0.000001;0.997534;,
  0.062689;0.662532;0.746405;,
  0.110557;-0.692322;0.713069;,
  0.163525;-0.000001;0.986539;,
  0.121412;0.680349;0.722762;,
  0.065417;0.923896;0.377011;,
  -0.597487;0.627584;-0.499148;,
  -0.665285;0.000001;-0.746590;,
  -0.597710;-0.587986;-0.544991;,
  -0.563694;-0.729781;-0.386870;,
  -0.910801;0.320822;-0.259836;,
  -0.929060;0.000153;-0.369929;,
  -0.909156;-0.303631;-0.285032;,
  -0.897692;-0.390759;-0.203607;,
  -0.086840;0.711604;-0.697193;,
  -0.111936;0.000004;-0.993715;,
  -0.088093;-0.706380;-0.702329;,
  -0.063382;0.133822;-0.988976;,
  0.063535;0.913976;0.400762;,
  0.113510;0.671241;0.732496;,
  0.151664;-0.000001;0.988432;,
  0.107147;-0.678133;0.727087;,
  0.223305;-0.974749;-0.000000;,
  0.269395;-0.688437;-0.673409;,
  0.290342;-0.001274;-0.956922;,
  0.283909;0.649029;-0.705803;,
  0.249672;0.883032;-0.397390;,
  0.269394;-0.688437;0.673410;,
  0.290341;-0.001273;0.956922;,
  0.280740;0.655185;0.701369;,
  0.242159;0.890555;0.385061;,
  0.000000;0.000000;1.000000;,
  0.567111;-0.545210;-0.617358;,
  0.037018;-0.000001;-0.999315;,
  0.567112;0.545210;-0.617358;,
  0.567112;0.545210;0.617358;,
  0.037018;-0.000001;0.999315;,
  0.567111;-0.545210;0.617358;,
  1.000000;-0.000001;-0.000000;,
  0.940008;-0.341154;-0.000000;,
  0.927246;-0.246493;-0.281881;,
  0.933677;-0.000001;-0.358117;,
  0.927246;0.246492;-0.281880;,
  0.940007;0.341154;0.000000;,
  0.927246;-0.246493;0.281881;,
  0.933677;-0.000001;0.358117;,
  0.927246;0.246492;0.281880;,
  0.037264;0.883681;0.466605;,
  0.043557;0.395137;0.917589;,
  0.075843;0.847032;0.526103;,
  0.068499;0.935476;0.346687;,
  0.065967;-0.918529;0.389812;,
  0.066839;-0.905849;0.418293;,
  0.076411;0.847013;-0.526052;,
  0.068873;0.935458;-0.346664;,
  0.066020;-0.919315;-0.387944;,
  0.066997;-0.906486;-0.416887;,
  -0.925747;0.269896;0.264856;,
  0.264582;-0.038079;0.963611;,
  -0.264562;0.038149;-0.963614;,
  -0.049550;-0.898553;0.436058;,
  -0.056589;-0.898219;0.435890;,
  1.000000;0.000000;0.000000;,
  -0.063626;-0.897841;0.435699;,
  -0.108114;0.434021;0.894392;,
  0.063626;0.897843;-0.435695;,
  0.108115;-0.434032;-0.894387;,
  0.048888;0.892607;0.448178;,
  0.057571;0.892107;0.448142;,
  0.994177;-0.104169;-0.027582;,
  0.066250;0.891539;0.448073;,
  -0.085503;0.456283;-0.885717;,
  -0.066248;-0.891541;-0.448069;,
  0.085503;-0.456273;0.885722;,
  -0.474433;-0.880292;0.000000;,
  0.071631;-0.997431;0.001017;,
  0.065029;-0.930315;0.360951;,
  -0.058328;0.000000;0.998297;,
  -0.115593;0.121859;0.985793;,
  -0.121437;0.141293;0.982491;,
  -0.007127;0.000000;0.999975;,
  -0.121695;0.183182;0.975518;,
  -0.073104;0.997324;0.000000;,
  -0.068356;-0.887900;0.454930;,
  -0.217354;0.133877;-0.966868;,
  -0.000544;0.395015;-0.918675;,
  0.044181;0.394481;-0.917842;,
  0.064977;-0.931223;-0.358612;,
  0.000000;0.000000;-1.000000;,
  -0.103709;0.133766;-0.985572;,
  -0.091399;0.119907;-0.988569;,
  -0.052019;0.000000;-0.998646;,
  0.000064;-0.000237;-1.000000;,
  -0.007127;0.000000;-0.999975;,
  -0.091753;0.180245;-0.979333;,
  -0.068356;-0.887900;-0.454930;,
  0.966399;0.174438;0.188797;,
  0.967936;0.251196;0.000000;,
  -0.967340;-0.253484;0.000000;,
  -0.966599;-0.175700;-0.186590;,
  0.957173;-0.000509;0.289515;,
  -0.957168;0.000483;-0.289531;,
  0.965239;-0.171072;0.197604;,
  -0.964933;0.168615;-0.201182;,
  0.964536;-0.242886;0.103329;,
  -0.964896;0.238212;-0.110592;,
  -0.075151;0.359868;0.929972;,
  -0.966599;-0.175700;0.186590;,
  0.966408;0.174429;-0.188758;,
  -0.957168;0.000483;0.289531;,
  0.957185;-0.000517;-0.289477;,
  -0.965126;0.170367;0.198761;,
  0.965302;-0.171673;-0.196777;,
  -0.047262;0.354300;-0.933937;,
  -0.964614;0.241577;0.105644;,
  0.964424;-0.244039;-0.101638;,
  0.610960;-0.791661;-0.000000;,
  0.610960;0.791661;0.000000;,
  0.898702;-0.000001;-0.438560;,
  0.898702;-0.000001;0.438560;,
  -0.999998;-0.002091;-0.000225;,
  -0.000191;0.000711;1.000000;,
  0.995792;0.091640;0.000000;,
  0.992882;0.105768;0.054752;,
  0.996029;0.000026;0.089035;,
  0.991675;-0.109472;0.067796;,
  0.994268;-0.099065;0.040226;,
  -0.090561;0.415107;0.905254;,
  0.992882;0.105768;-0.054752;,
  0.996111;0.000022;-0.088106;,
  0.991948;-0.108369;-0.065540;,
  -0.082617;0.393937;-0.915417;,
  0.994591;-0.096847;-0.037556;;
  186;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  4;130,130,130,130;,
  4;131,131,131,131;,
  4;132,132,132,132;,
  4;3,4,4,3;,
  4;5,6,6,5;,
  4;7,8,8,7;,
  4;133,134,134,133;,
  4;9,9,9,9;,
  4;135,135,135,135;,
  4;134,134,136,136;,
  4;8,8,137,137;,
  4;6,6,138,138;,
  4;4,4,139,139;,
  4;10,11,11,10;,
  4;12,13,13,12;,
  4;14,15,15,14;,
  4;140,141,141,140;,
  4;16,16,16,16;,
  4;142,142,142,142;,
  4;141,141,143,143;,
  4;15,15,144,144;,
  4;13,13,145,145;,
  4;11,11,146,146;,
  4;18,17,22,23;,
  4;19,18,23,24;,
  4;20,19,24,25;,
  4;21,20,25,26;,
  3;23,22,27;,
  3;24,23,27;,
  3;25,24,27;,
  3;26,25,147;,
  3;111,112,113;,
  3;111,113,114;,
  3;111,114,115;,
  3;111,115,116;,
  4;29,28,33,34;,
  4;30,29,34,35;,
  4;31,30,35,36;,
  4;32,31,36,37;,
  4;62,53,38,39;,
  4;61,62,39,40;,
  4;60,61,40,41;,
  4;59,60,41,42;,
  4;96,95,95,96;,
  4;97,96,96,97;,
  4;98,97,97,98;,
  4;99,98,98,99;,
  4;44,43,48,49;,
  4;45,44,49,50;,
  4;46,45,50,51;,
  4;47,46,51,52;,
  3;120,121,122;,
  3;120,122,123;,
  3;148,149,124;,
  3;148,124,125;,
  4;104,104,104,27;,
  4;58,150,151,152;,
  4;104,54,54,104;,
  3;104,54,104;,
  4;104,104,104,104;,
  4;104,104,153,153;,
  3;152,151,154;,
  4;104,27,104,104;,
  4;104,104,54,104;,
  4;55,155,17,18;,
  4;56,55,18,19;,
  4;57,56,19,20;,
  4;156,57,20,21;,
  4;153,153,150,58;,
  4;37,36,60,59;,
  4;36,35,61,60;,
  4;35,34,62,61;,
  4;34,33,53,62;,
  4;63,66,22,17;,
  4;64,67,66,63;,
  4;65,68,67,64;,
  4;21,26,68,65;,
  3;66,157,22;,
  3;67,157,66;,
  3;68,157,67;,
  3;26,147,68;,
  3;111,117,112;,
  3;111,118,117;,
  3;111,119,118;,
  3;111,116,119;,
  4;69,72,33,28;,
  4;70,73,72,69;,
  4;71,74,73,70;,
  4;32,37,74,71;,
  4;94,75,38,53;,
  4;93,76,75,94;,
  4;92,77,76,93;,
  4;91,78,77,92;,
  4;100,100,95,95;,
  4;101,101,100,100;,
  4;102,102,101,101;,
  4;103,103,102,102;,
  4;79,83,48,43;,
  4;80,84,83,79;,
  4;81,85,84,80;,
  4;82,86,85,81;,
  3;158,126,159;,
  3;120,127,126;,
  3;148,128,160;,
  3;148,129,128;,
  4;161,157,161,161;,
  4;90,162,163,164;,
  4;161,161,165,165;,
  3;161,161,165;,
  4;161,161,161,161;,
  4;161,166,166,161;,
  3;162,167,163;,
  4;161,161,161,157;,
  4;161,161,165,161;,
  4;87,63,17,155;,
  4;88,64,63,87;,
  4;89,65,64,88;,
  4;168,21,65,89;,
  4;166,90,164,166;,
  4;37,91,92,74;,
  4;74,92,93,73;,
  4;73,93,94,72;,
  4;72,94,53,33;,
  4;169,170,170,169;,
  4;104,104,104,104;,
  4;171,172,172,171;,
  4;173,169,169,173;,
  4;172,174,174,172;,
  4;175,173,173,175;,
  4;174,176,176,174;,
  4;177,175,175,177;,
  4;176,178,178,176;,
  4;154,151,179,179;,
  4;180,171,171,180;,
  4;161,161,161,161;,
  4;170,181,181,170;,
  4;182,180,180,182;,
  4;181,183,183,181;,
  4;184,182,182,184;,
  4;183,185,185,183;,
  4;163,167,186,186;,
  4;187,184,184,187;,
  4;185,188,188,185;,
  4;105,189,28,29;,
  4;106,105,29,30;,
  4;107,106,30,31;,
  4;190,107,31,32;,
  4;108,190,32,71;,
  4;109,108,71,70;,
  4;110,109,70,69;,
  4;189,110,69,28;,
  4;27,104,104,104;,
  4;189,105,113,112;,
  4;105,191,114,113;,
  4;191,107,115,114;,
  4;107,190,116,115;,
  4;104,27,104,104;,
  4;110,189,112,117;,
  4;161,157,161,161;,
  4;192,110,117,118;,
  4;108,192,118,119;,
  4;157,161,161,161;,
  4;190,108,119,116;,
  3;49,48,193;,
  4;54,54,121,194;,
  4;195,196,196,195;,
  3;50,49,193;,
  4;196,197,197,196;,
  3;51,50,193;,
  4;197,198,198,197;,
  3;52,51,193;,
  4;198,199,199,198;,
  4;58,152,120,200;,
  4;201,195,195,201;,
  4;165,165,158,159;,
  3;48,83,193;,
  4;202,201,201,202;,
  3;83,84,193;,
  4;203,202,202,203;,
  3;84,85,193;,
  4;162,90,204,158;,
  4;205,203,203,205;,
  3;85,86,193;;
 }
 MeshTextureCoords {
  155;
  0.248531;0.451632;,
  0.235198;0.319489;,
  0.078231;0.441687;,
  0.091564;0.573829;,
  0.095987;0.572220;,
  0.082654;0.440078;,
  0.239622;0.317881;,
  0.252955;0.450023;,
  0.083760;0.540801;,
  0.486413;0.567847;,
  0.486413;0.613741;,
  0.083760;0.586695;,
  0.486413;0.520959;,
  0.083760;0.493913;,
  0.486413;0.475065;,
  0.083760;0.448019;,
  0.916367;0.714879;,
  0.916367;0.622097;,
  0.916367;0.667991;,
  0.916367;0.760773;,
  0.084531;0.385661;,
  0.487423;0.364609;,
  0.491498;0.411632;,
  0.088606;0.432684;,
  0.490659;0.503824;,
  0.087768;0.524877;,
  0.486585;0.456800;,
  0.083694;0.477853;,
  0.917694;0.239227;,
  0.916856;0.331419;,
  0.920930;0.378443;,
  0.921768;0.286251;,
  0.446065;0.536482;,
  0.446065;0.551679;,
  0.204390;0.566875;,
  0.204390;0.547228;,
  0.446065;0.499794;,
  0.204390;0.499794;,
  0.446065;0.463105;,
  0.204390;0.452359;,
  0.446065;0.447908;,
  0.204390;0.432711;,
  0.105489;0.499794;,
  0.068027;0.499793;,
  0.083795;0.572851;,
  0.083795;0.541477;,
  0.083795;0.499793;,
  0.083795;0.458110;,
  0.083795;0.426736;,
  0.204390;0.599113;,
  0.204390;0.640253;,
  0.446065;0.655450;,
  0.446065;0.609859;,
  0.204390;0.499794;,
  0.446065;0.499794;,
  0.204390;0.400474;,
  0.446065;0.389728;,
  0.204390;0.359334;,
  0.446065;0.344137;,
  0.606143;0.615903;,
  0.606143;0.663997;,
  0.778290;0.669168;,
  0.778290;0.619560;,
  0.606143;0.499794;,
  0.778290;0.499794;,
  0.606143;0.383684;,
  0.778290;0.380027;,
  0.606143;0.335589;,
  0.778290;0.330418;,
  0.773523;0.634750;,
  0.774137;0.692056;,
  0.927820;0.734721;,
  0.927206;0.664918;,
  0.772909;0.499794;,
  0.926591;0.499794;,
  0.773352;0.365044;,
  0.927034;0.334875;,
  0.773795;0.307943;,
  0.927477;0.265278;,
  0.931973;0.649728;,
  0.931973;0.711833;,
  0.778290;0.580686;,
  0.778290;0.556993;,
  0.931973;0.499794;,
  0.778290;0.499794;,
  0.931973;0.349859;,
  0.778290;0.442594;,
  0.931973;0.287754;,
  0.778290;0.418901;,
  0.782058;0.499081;,
  0.608738;0.523318;,
  0.609674;0.509730;,
  0.610610;0.499794;,
  0.610184;0.488382;,
  0.609759;0.473319;,
  0.105489;0.631958;,
  0.606143;0.433822;,
  0.606143;0.565764;,
  0.105489;0.367629;,
  0.606143;0.546442;,
  0.606143;0.499794;,
  0.606143;0.453145;,
  0.446065;0.536482;,
  0.204390;0.547228;,
  0.446065;0.499794;,
  0.204390;0.499794;,
  0.446065;0.463105;,
  0.204390;0.452359;,
  0.083795;0.541477;,
  0.083795;0.499793;,
  0.083795;0.458110;,
  0.204390;0.599113;,
  0.446065;0.609859;,
  0.204390;0.499794;,
  0.446065;0.499794;,
  0.204390;0.400474;,
  0.446065;0.389728;,
  0.606143;0.615903;,
  0.778290;0.619560;,
  0.606143;0.499794;,
  0.778290;0.499794;,
  0.606143;0.383684;,
  0.778290;0.380027;,
  0.606143;0.335589;,
  0.778290;0.330418;,
  0.773523;0.634750;,
  0.927206;0.664918;,
  0.772909;0.499794;,
  0.926591;0.499794;,
  0.773352;0.365044;,
  0.927034;0.334875;,
  0.773795;0.307943;,
  0.927477;0.265278;,
  0.931973;0.649728;,
  0.778290;0.556993;,
  0.931973;0.499794;,
  0.778290;0.499794;,
  0.931973;0.349859;,
  0.778290;0.442594;,
  0.931973;0.287754;,
  0.778290;0.418901;,
  0.609674;0.509730;,
  0.610610;0.499794;,
  0.610111;0.488587;,
  0.609611;0.473729;,
  0.606143;0.433822;,
  0.606143;0.546442;,
  0.606143;0.499794;,
  0.606143;0.453145;,
  0.105489;0.575201;,
  0.105489;0.499794;,
  0.105489;0.424385;,
  0.105489;0.424385;,
  0.105489;0.499794;,
  0.105489;0.575201;;
 }
}