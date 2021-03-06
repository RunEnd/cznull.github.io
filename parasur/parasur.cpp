// parasur.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "parasur.h"
#include "vec.h"

#define MAX_LOADSTRING 100

#define MapSize 1048580

#define cal(cx,ox,nox) \
	parasur.cx[0] = u;\
	parasur.cx[1] = v;\
	for (i = 0; i < parasur.nox; i++) {\
		switch (parasur.ox[i].com){\
		case '+':\
			parasur.cx[parasur.ox[i].num3] = parasur.cx[parasur.ox[i].num1] + parasur.cx[parasur.ox[i].num2];\
			break;\
		case 44:\
			parasur.cx[parasur.ox[i].num3] = -parasur.cx[parasur.ox[i].num1];\
			break;\
		case '-':\
			parasur.cx[parasur.ox[i].num3] = parasur.cx[parasur.ox[i].num1] - parasur.cx[parasur.ox[i].num2];\
			break;\
		case '*':\
			parasur.cx[parasur.ox[i].num3] = parasur.cx[parasur.ox[i].num1] * parasur.cx[parasur.ox[i].num2];\
			break;\
		case '/':\
			parasur.cx[parasur.ox[i].num3] = parasur.cx[parasur.ox[i].num1] / parasur.cx[parasur.ox[i].num2];\
			break;\
		case 257:\
			parasur.cx[parasur.ox[i].num3] = cos(parasur.cx[parasur.ox[i].num1]);\
			break;\
		case 258:\
			parasur.cx[parasur.ox[i].num3] = sin(parasur.cx[parasur.ox[i].num1]);\
			break;\
		case 259:\
			parasur.cx[parasur.ox[i].num3] = sqrt(parasur.cx[parasur.ox[i].num1]);\
			break;\
		case 260:\
			parasur.cx[parasur.ox[i].num3] = log(parasur.cx[parasur.ox[i].num1]);\
			break;\
		case 261:\
			parasur.cx[parasur.ox[i].num3] = tan(parasur.cx[parasur.ox[i].num1]);\
			break;\
		case 262:\
			parasur.cx[parasur.ox[i].num3] = exp(parasur.cx[parasur.ox[i].num1]);\
			break;\
		}\
	}\

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HDC hdc1, hdc2;
HGLRC m_hrc;
int mx, my, cx, cy;
double ang1, ang2, len;
GLuint vbo, cbo, tbo, nbo, versha, frasha, pro;

const char *shaderv = 
"varying vec4 fc;"
"varying vec4 a;"
"float l;"
"varying vec3 ntemp;"
"void main() {"
"	a = gl_Vertex;"
"	ntemp = gl_Normal;"
"	ntemp = gl_NormalMatrix * ntemp;"
"	a = gl_ModelViewMatrix * a;"
"	fc = gl_Color;"
"	gl_Position = gl_ProjectionMatrix * a;"
"}";

const char *shaderf = 
"varying vec4 fc;"
"float s,t;"
"vec3 texdir;"
"varying vec4 a;"
"vec3 b;"
"varying vec3 ntemp;"
"void main(){"
"	s=a.x*a.x+a.y*a.y+a.z*a.z;"
"	s=1.0/sqrt(s);"
"	b.x=a.x*s;"
"	b.y=a.y*s;"
"	b.z=a.z*s;"
"	texdir=reflect(b,ntemp);"
"	s=texdir.x*0.276+texdir.y*0.276+texdir.z*0.920;"
"	t=ntemp.x*0.276+ntemp.y*0.276+ntemp.z*0.920;"
"	t=(gl_FrontFacing)?t:-t;"
"	s=(s>0.0)?s:0.0;"
"	s=s*s;"
"	s=s*s*0.4+t*0.25+0.3;"
"	gl_FragColor.r = fc.r*s;"
"	gl_FragColor.g = fc.g*s;"
"	gl_FragColor.b = fc.b*s;"
"	gl_FragColor.a = fc.a;"
"}";

int getcoms(express &com, char *x, char *y, char *z);

express parasur = { 0 };

const int noqi=64;
int Noq=4096;
double coslim = 1.0 - 1.0 / 24, aculim = 0.001;

int quaMap[MapSize], Maplef[MapSize], Mapbot[MapSize], Maprig[MapSize], Maptop[MapSize], Mapfat[MapSize];
double quau[MapSize], quav[MapSize], qual[MapSize];
int end, vercount;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK parainput(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

struct buff {
	float3 *data;
	int max, end;
	buff(int n) {
		data = (float3*)malloc(sizeof(float3)*n);
		max = n;
		end = 0;
	}
	~buff(void) {
		free(data);
	}
	void push_back(float3 x);
};

void buff::push_back(float3 x) {
	if (end == max) {
		float3 *p = (float3*)malloc(sizeof(float3)*max * 2);
		memcpy(p, data, sizeof(float3)*max);
		free(data);
		data = p;
	}
	data[end] = x;
	end++;
}

inline double getx(double u, double v) {
	return (2 * u - 1);
}

inline double gety(double u, double v) {
	return (2 * v - 1);
}

inline double getz(double u, double v) {
	return cos(((u * 2 - 1)*(u * 2 - 1) - (v * 2 - 1)*(v * 2 - 1)) * 1 * 6.283185)*sqrt(1 - (u * 2 - 1)*(u * 2 - 1) - (v * 2 - 1)*(v * 2 - 1));
}

inline float getr(double u, double v) {
	//return 1.0f;
	//return float(rand() % 256) / 256;
	return sin(v*3.1416) / 2 + 0.5;
}

inline float getg(double u, double v) {
	//return 1.0f;
	//return float(rand() % 256) / 256;
	return sin(u*3.1416) / 2 + 0.5;
}

inline float getb(double u, double v) {
	return 1.0f;
	//return float(rand() % 256) / 256;
	return cos(u*6.2832*2)*0.5 + 0.5;
}

inline double3 getv(double u, double v) {
	int i;
	u = u * (parasur.uh - parasur.ul) + parasur.ul;
	v = v * (parasur.vh - parasur.vl) + parasur.vl;
	parasur.cx[0] = u;
	parasur.cx[1] = v;
	cal(cx, ox, nox);
	cal(cy, oy, noy);
	cal(cz, oz, noz);
	return { parasur.cx[parasur.outx],parasur.cy[parasur.outy],parasur.cz[parasur.outz] };
}

inline float3 getc(double u, double v) {
	return { getr(u, v) ,getg(u, v) ,getb(u, v) };
}

int invalid(double3 *vertex) {
	if ((*(unsigned long long*)(vertex) & 0x7ff0000000000000) == 0x7ff0000000000000) {
		return 1;
	}
	if ((*((unsigned long long*)(vertex) + 1) & 0x7ff0000000000000) == 0x7ff0000000000000) {
		return 1;
	}
	if ((*((unsigned long long*)(vertex) + 2) & 0x7ff0000000000000) == 0x7ff0000000000000) {
		return 1;
	}
	return 0;
}

double3 getn1(double u, double v, double ltemp, double* dl) {
	double la, lb, absa, absb, left, right;
	double3 vertexo, vertexa, vertexb, veca, vecb;
	la = ltemp;
	lb = ltemp;
	left = 0.0;
	right = 1.0;
	int i;
	vertexo = getv(u, v);
	vertexa = getv(u + la * dl[0], v + la * dl[1]);
	vertexb = getv(u + lb * dl[2], v + lb * dl[3]);
	veca = vertexa - vertexo;
	vecb = vertexb - vertexo;
	absa = norm(veca);
	absb = norm(vecb);
	if (absa > absb) {
		for (i = 0; i<10; i++) {
			la = ltemp * (left + right);
			vertexa = getv(u + la * dl[0], v + la * dl[1]);
			veca = vertexa - vertexo;
			absa = norm(veca);
			if (absa > absb) {
				right = 0.5*(left + right);
			}
			else {
				left = 0.5*(left + right);
			}
		}
	}
	else {
		for (i = 0; i<10; i++) {
			lb = ltemp * (left + right);
			vertexb = getv(u + lb * dl[2], v + lb * dl[3]);
			vecb = vertexb - vertexo;
			absb = norm(vecb);
			if (absb > absa) {
				right = 0.5*(left + right);
			}
			else {
				left = 0.5*(left + right);
			}
		}
	}
	vertexo = cross(veca, vecb);
	return vertexo * (1.0 / sqrt(norm(vertexo)));
}

double3 getn(double u, double v, double l) {
	//return { 0,0,0 };
	double abnor, ltemp, ltemp1, x0;
	double3 vec[2], nor;
	int i, isnan;
	ltemp = l / 16;
	vec[0] = getv(u + ltemp, v) - getv(u - ltemp, v);
	vec[1] = getv(u, v + ltemp) - getv(u, v - ltemp);
	if (!invalid(vec) && !invalid(vec + 1)) {
		ltemp /= 4;
		vec[0] = getv(u + ltemp, v) - getv(u - ltemp, v);
		vec[1] = getv(u, v + ltemp) - getv(u, v - ltemp);
		nor = cross(vec[0], vec[1]);
		return nor * (1.0 / sqrt(norm(nor)));
	}
	else {
		ltemp = l / 8;
		while (ltemp > l*1.0e-4) {
			double dl[4] = { 0.9659,0.2588,0.2588,0.9659 };
			double dlt;
			vec[0] = getv(u + ltemp, v);
			vec[1] = getv(u, v + ltemp);
			if (!invalid(vec) && !invalid(vec + 1)) {
				return getn1(u, v, ltemp / 2, dl);
			}
			for (i = 0; i < 2; i++) {
				dlt = 0.5*dl[i * 2] - 0.86603*dl[i * 2 + 1];
				dl[i * 2 + 1] = 0.5*dl[i * 2 + 1] + 0.86603*dl[i * 2];
				dl[i * 2] = dlt;
			}
			vec[0] = getv(u + ltemp * 0.5, v + ltemp * 0.866);
			vec[1] = getv(u - ltemp * 0.866, v + ltemp * 0.5);
			if (!invalid(vec) && !invalid(vec + 1)) {
				return getn1(u, v, ltemp / 2, dl);
			}
			for (i = 0; i < 2; i++) {
				dlt = 0.5*dl[i * 2] - 0.86603*dl[i * 2 + 1];
				dl[i * 2 + 1] = 0.5*dl[i * 2 + 1] + 0.86603*dl[i * 2];
				dl[i * 2] = dlt;
			}
			vec[0] = getv(u - ltemp * 0.5, v + ltemp * 0.866);
			vec[1] = getv(u - ltemp * 0.866, v - ltemp * 0.5);
			if (!invalid(vec) && !invalid(vec + 1)) {
				return getn1(u, v, ltemp / 2, dl);
			}
			for (i = 0; i < 2; i++) {
				dlt = 0.5*dl[i * 2] - 0.86603*dl[i * 2 + 1];
				dl[i * 2 + 1] = 0.5*dl[i * 2 + 1] + 0.86603*dl[i * 2];
				dl[i * 2] = dlt;
			}
			vec[0] = getv(u - ltemp, v);
			vec[1] = getv(u, v - ltemp);
			if (!invalid(vec) && !invalid(vec + 1)) {
				return getn1(u, v, ltemp / 2, dl);
			}
			for (i = 0; i < 2; i++) {
				dlt = 0.5*dl[i * 2] - 0.86603*dl[i * 2 + 1];
				dl[i * 2 + 1] = 0.5*dl[i * 2 + 1] + 0.86603*dl[i * 2];
				dl[i * 2] = dlt;
			}
			vec[0] = getv(u - ltemp * 0.5, v - ltemp * 0.866);
			vec[1] = getv(u + ltemp * 0.866, v - ltemp * 0.5);
			if (!invalid(vec) && !invalid(vec + 1)) {
				return getn1(u, v, ltemp / 2, dl);
			}
			for (i = 0; i < 2; i++) {
				dlt = 0.5*dl[i * 2] - 0.86603*dl[i * 2 + 1];
				dl[i * 2 + 1] = 0.5*dl[i * 2 + 1] + 0.86603*dl[i * 2];
				dl[i * 2] = dlt;
			}
			vec[0] = getv(u + ltemp * 0.5, v - ltemp * 0.866);
			vec[1] = getv(u + ltemp * 0.866, v + ltemp * 0.5);
			if (!invalid(vec) && !invalid(vec + 1)) {
				return getn1(u, v, ltemp / 2, dl);
			}
			ltemp /= 2;
		}
		return { 1.0,0.0,0.0 };
	}
}

void invtri1(buff &vertexs, buff &colors, buff &normals, double3 v1, double3 v2, double3 n1, double3 n2, double2 c1, double2 c2, double2 c3, double l) {
	double left, right,mid;
	double2 c4, c5;
	double3 v4, v5, n4, n5;
	left = 0;
	right = 1;
	while ((right - left) > (1.0e-6)) {
		mid = (left + right) / 2;
		c4 = c3 * mid + c1 * (1.0 - mid);
		v4 = getv(c4.x, c4.y);
		if (invalid(&v4)) {
			right = mid;
		}
		else {
			left = mid;
		}
	}
	c4 = c3 * left + c1 * (1.0 - left);

	left = 0;
	right = 1;
	while ((right - left) > (1.0e-6)) {
		mid = (left + right) / 2;
		c5 = c3 * mid + c2 * (1.0 - mid);
		v5 = getv(c5.x, c5.y);
		if (invalid(&v5)) {
			right = mid;
		}
		else {
			left = mid;
		}
	}
	c5 = c3 * left + c2 * (1.0 - left);

	v4 = getv(c4.x, c4.y);
	n4 = getn(c4.x, c4.y, l);
	v5 = getv(c5.x, c5.y);
	n5 = getn(c5.x, c5.y, l);
	vertexs.push_back(v1);
	vertexs.push_back(v2);
	vertexs.push_back(v5);
	normals.push_back(n1);
	normals.push_back(n2);
	normals.push_back(n5);
	colors.push_back(getc(c1.x, c1.y));
	colors.push_back(getc(c2.x, c2.y));
	colors.push_back(getc(c5.x, c5.y));
	vertexs.push_back(v1);
	vertexs.push_back(v5);
	vertexs.push_back(v4);
	normals.push_back(n1);
	normals.push_back(n5);
	normals.push_back(n4);
	colors.push_back(getc(c1.x, c1.y));
	colors.push_back(getc(c5.x, c5.y));
	colors.push_back(getc(c4.x, c4.y));
}

void invtri2(buff &vertexs, buff &colors, buff &normals, double3 v1, double3 n1, double2 c1, double2 c2, double2 c3, double l) {
	double left, right, mid;
	double2 c4, c5;
	double3 v4, v5, n4, n5;
	left = 0;
	right = 1;
	while ((right - left) > (1.0e-6)) {
		mid = (left + right) / 2;
		c4 = c2 * mid + c1 * (1.0 - mid);
		v4 = getv(c4.x, c4.y);
		if (invalid(&v4)) {
			right = mid;
		}
		else {
			left = mid;
		}
	}
	c4 = c2 * left + c1 * (1.0 - left);

	left = 0;
	right = 1;
	while ((right - left) > (1.0e-6)) {
		mid = (left + right) / 2;
		c5 = c3 * mid + c1 * (1.0 - mid);
		v5 = getv(c5.x, c5.y);
		if (invalid(&v5)) {
			right = mid;
		}
		else {
			left = mid;
		}
	}
	c5 = c3 * left + c1 * (1.0 - left);

	v4 = getv(c4.x, c4.y);
	n4 = getn(c4.x, c4.y, l);
	v5 = getv(c5.x, c5.y);
	n5 = getn(c5.x, c5.y, l);
	vertexs.push_back(v1);
	vertexs.push_back(v4);
	vertexs.push_back(v5);
	normals.push_back(n1);
	normals.push_back(n4);
	normals.push_back(n5);
	colors.push_back(getc(c1.x, c1.y));
	colors.push_back(getc(c4.x, c4.y));
	colors.push_back(getc(c5.x, c5.y));
}

inline void tri1(buff &vertexs, buff &colors, buff &normals, double3 v1, double3 v2, double3 v3,double3 n1,double3 n2,double3 n3,double2 c1,double2 c2,double2 c3,double l) {
	int inv[3], invcount;
	inv[0] = invalid(&v1);
	inv[1] = invalid(&v2);
	inv[2] = invalid(&v3);
	invcount = inv[0] + inv[1] + inv[2];
	switch (invcount) {
	case 0: {
		vertexs.push_back(v1);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		normals.push_back(n1);
		normals.push_back(n2);
		normals.push_back(n3);
		colors.push_back(getc(c1.x, c1.y));
		colors.push_back(getc(c2.x, c2.y));
		colors.push_back(getc(c3.x, c3.y));
		break;
	}
	case 1: {
		if (inv[0]) {
			invtri1(vertexs, colors, normals, v2, v3, n2, n3, c2, c3, c1, l);
		}
		if (inv[1]) {
			invtri1(vertexs, colors, normals, v3, v1, n3, n1, c3, c1, c2, l);
		}
		if (inv[2]) {
			invtri1(vertexs, colors, normals, v1, v2, n1, n2, c1, c2, c3, l);
		}
		break;
	}
	case 2: {
		if (!inv[0]) {
			invtri2(vertexs, colors, normals, v1, n1, c1, c2, c3, l);
		}
		if (!inv[1]) {
			invtri2(vertexs, colors, normals, v2, n2, c2, c3, c1, l);
		}
		if (!inv[2]) {
			invtri2(vertexs, colors, normals, v3, n3, c3, c1, c2, l);
		}
		break;
	}
	}
}

inline void tri2(buff &vertexs, buff &colors, buff &normals, double3 v1, double3 v2, double3 v3, double3 n1, double3 n2, double3 n3, double2 c1, double2 c2, double2 c3, double l) {
	double3 v4, n4;
	double2 c4;
	int inv[4], invcountl, invcountr;
	c4 = { (c1.x + c2.x) / 2, (c1.y + c2.y) / 2 };
	v4 = getv(c4.x, c4.y);
	inv[0] = invalid(&v1);
	inv[1] = invalid(&v2);
	inv[2] = invalid(&v3);
	inv[3] = invalid(&v4);
	if (!inv[3]) {
		n4 = getn(c4.x, c4.y, l);
	}
	invcountl = inv[0] + inv[3] + inv[2];
	invcountr = inv[3] + inv[1] + inv[2];
	switch (invcountl) {
	case 0: {
		vertexs.push_back(v1);
		vertexs.push_back(v4);
		vertexs.push_back(v3);
		normals.push_back(n1);
		normals.push_back(n4);
		normals.push_back(n3);
		colors.push_back(getc(c1.x, c1.y));
		colors.push_back(getc((c1.x + c2.x) / 2, (c1.y + c2.y) / 2));
		colors.push_back(getc(c3.x, c3.y));
		break;
	}
	case 1: {
		if (inv[0]) {
			invtri1(vertexs, colors, normals, v4, v3, n4, n3, c4, c3, c1, l);
		}
		if (inv[3]) {
			invtri1(vertexs, colors, normals, v3, v1, n3, n1, c3, c1, c4, l);
		}
		if (inv[2]) {
			invtri1(vertexs, colors, normals, v1, v4, n1, n4, c1, c4, c3, l);
		}
		break;
	}
	case 2: {
		if (!inv[0]) {
			invtri2(vertexs, colors, normals, v1, n1, c1, c4, c3, l);
		}
		if (!inv[3]) {
			invtri2(vertexs, colors, normals, v4, n4, c4, c3, c1, l);
		}
		if (!inv[2]) {
			invtri2(vertexs, colors, normals, v3, n3, c3, c1, c4, l);
		}
		break;
	}
	}
	switch (invcountl) {
	case 0: {
		vertexs.push_back(v4);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		normals.push_back(n4);
		normals.push_back(n2);
		normals.push_back(n3);
		colors.push_back(getc((c1.x + c2.x) / 2, (c1.y + c2.y) / 2));
		colors.push_back(getc(c2.x, c2.y));
		colors.push_back(getc(c3.x, c3.y));
		break;
	}
	case 1: {
		if (inv[3]) {
			invtri1(vertexs, colors, normals, v2, v3, n2, n3, c2, c3, c4, l);
		}
		if (inv[1]) {
			invtri1(vertexs, colors, normals, v3, v4, n3, n4, c3, c4, c2, l);
		}
		if (inv[2]) {
			invtri1(vertexs, colors, normals, v4, v2, n4, n2, c4, c2, c3, l);
		}
		break;
	}
	case 2: {
		if (!inv[3]) {
			invtri2(vertexs, colors, normals, v4, n4, c4, c2, c3, l);
		}
		if (!inv[1]) {
			invtri2(vertexs, colors, normals, v2, n2, c2, c3, c4, l);
		}
		if (!inv[2]) {
			invtri2(vertexs, colors, normals, v3, n3, c3, c4, c2, l);
		}
		break;
	}
	}
}

void scan() {
	buff vertexs(12 * end), colors(12 * end), normals(12 * end), texcoords(12 * end);
	int i;
	double u, v, l;
	double3 vertex[5], normal[5];
	double2 c[5];
	for (i = 4; i < end; i++) {
		if (quaMap[i] == 0) {
			u = quau[i];
			v = quav[i];
			l = qual[i];
			vertex[0] = getv(u, v);
			vertex[1] = getv(u + l, v);
			vertex[2] = getv(u + l, v + l);
			vertex[3] = getv(u, v + l);
			vertex[4] = getv(u + l / 2, v + l / 2);
			if (!invalid(vertex)) {
				normal[0] = getn(u, v, l);
			}
			if (!invalid(vertex + 1)) {
				normal[1] = getn(u + l, v, l);
			}
			if (!invalid(vertex + 2)) {
			normal[2] = getn(u + l, v + l, l);
			}
			if (!invalid(vertex + 3)) {
			normal[3] = getn(u, v + l, l);
			}
			if (!invalid(vertex + 4)) {
				normal[4] = getn(u + l / 2, v + l / 2, l);
			}

			c[0] = { u, v };
			c[1] = { u + l, v };
			c[2] = { u + l, v + l };
			c[3] = { u, v + l };
			c[4] = { u + l / 2, v + l / 2 };

			if (quaMap[Mapbot[i]]) {
				tri2(vertexs, colors, normals, vertex[0], vertex[1], vertex[4], normal[0], normal[1], normal[4], c[0], c[1], c[4], l);
			}
			else {
				tri1(vertexs, colors, normals, vertex[0], vertex[1], vertex[4], normal[0], normal[1], normal[4], c[0], c[1], c[4], l);
			}
			if (quaMap[Maprig[i]]) {
				tri2(vertexs, colors, normals, vertex[1], vertex[2], vertex[4], normal[1], normal[2], normal[4], c[1], c[2], c[4], l);
			}
			else {
				tri1(vertexs, colors, normals, vertex[1], vertex[2], vertex[4], normal[1], normal[2], normal[4], c[1], c[2], c[4], l);
			}
			if (quaMap[Maptop[i]]) {
				tri2(vertexs, colors, normals, vertex[2], vertex[3], vertex[4], normal[2], normal[3], normal[4], c[2], c[3], c[4], l);
			}
			else {
				tri1(vertexs, colors, normals, vertex[2], vertex[3], vertex[4], normal[2], normal[3], normal[4], c[2], c[3], c[4], l);
			}
			if (quaMap[Maplef[i]]) {
				tri2(vertexs, colors, normals, vertex[3], vertex[0], vertex[4], normal[3], normal[0], normal[4], c[3], c[0], c[4], l);
			}
			else {
				tri1(vertexs, colors, normals, vertex[3], vertex[0], vertex[4], normal[3], normal[0], normal[4], c[3], c[0], c[4], l);
			}
		}
	}
	vercount = normals.end;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vercount * sizeof(float3), vertexs.data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, vercount * sizeof(float3), colors.data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, vercount * sizeof(float3), normals.data, GL_STATIC_DRAW);
}

int needsep(double u, double v, double l) {
	double3 vertex[6], normal[5], vector[6];
	double sca[8], a, b, c, d, cosx[4];
	int i, invalidcount;

	invalidcount = 0;
	vertex[0] = getv(u, v);
	vertex[1] = getv(u + l, v);
	vertex[2] = getv(u + l, v + l);
	vertex[3] = getv(u, v + l);
	vertex[4] = getv(u + l / 2, v + l / 2);
	for (i = 0; i < 5; i++) {
		if (invalid(vertex + i)) {
			invalidcount++;
		}
	}
	if (invalidcount == 5) {
		return 0;
	}
	if (invalidcount) {
		return 1;
	}
	for (i = 0; i < 4; i++) {
		vector[i] = vertex[i] - vertex[4];
		sca[i] = norm(vector[i]);
	}
	a = max(sca[0], sca[1]);
	b = max(sca[2], sca[3]);
	c = max(a, b);
	normal[0] = cross(vector[0], vector[1]);
	normal[1] = cross(vector[1], vector[2]);
	normal[2] = cross(vector[2], vector[3]);
	normal[3] = cross(vector[3], vector[0]);
	for (i = 0; i < 4; i++) {
		sca[i] = sqrt(norm(normal[i]));
	}
	for (i = 1; i < 4; i++) {
		cosx[i] = dot(normal[i], normal[i - 1]) / (sca[i] * sca[i - 1]);
	}
	cosx[0] = dot(normal[0], normal[3]) / (sca[0] * sca[3]);
	a = min(cosx[0], cosx[1]);
	b = min(cosx[2], cosx[3]);
	d = min(a, b);
	if (d<coslim || (1 - d * d)>aculim*aculim / c / c) {
		return 1;
	}
	if ((v - 0.5*l) > 0) {
		vertex[5] = getv(u + l / 2, v - l * 0.5);
		vector[4] = vertex[0] - vertex[5];
		vector[5] = vertex[1] - vertex[5];
		normal[4] = cross(vector[5], vector[4]);
		a = dot(normal[0], normal[4]) / (sca[0] * sqrt(norm(normal[4])));
		d = min(d, a);
	}
	if ((u + 1.5* l) < 1) {
		vertex[5] = getv(u + l * 1.5, v + l *0.5);
		vector[4] = vertex[1] - vertex[5];
		vector[5] = vertex[2] - vertex[5];
		normal[4] = cross(vector[5], vector[4]);
		a = dot(normal[1], normal[4]) / (sca[1] * sqrt(norm(normal[4])));
		d = min(d, a);
	}
	if ((v + 1.5*l) < 1) {
		vertex[5] = getv(u + l * 0.5, v + l * 1.5);
		vector[4] = vertex[2] - vertex[5];
		vector[5] = vertex[3] - vertex[5];
		normal[4] = cross(vector[5], vector[4]);
		a = dot(normal[2], normal[4]) / (sca[2] * sqrt(norm(normal[4])));
		d = min(d, a);
	}
	if ((u - 0.5*l) > 0) {
		vertex[5] = getv(u - l * 0.5, v + l * 0.5);
		vector[4] = vertex[3] - vertex[5];
		vector[5] = vertex[0] - vertex[5];
		normal[4] = cross(vector[5], vector[4]);
		a = dot(normal[3], normal[4]) / (sca[3] * sqrt(norm(normal[4])));
		d = min(d, a);
	}
	if (d<coslim || (1 - d * d)>aculim*aculim / c / c) {
		return 1;
	}
	return 0;
}

void sepaquad(int cur, int force) {
	double u, v, l;
	u = quau[cur];
	v = quav[cur];
	l = qual[cur];
	if (l<(1.4 / Noq)) {
		return;
	}
	if (end > MapSize - 4) {
		return;
	}
	if (force | needsep(u, v, l)) {
		if (Mapbot[cur] < 4) {
			sepaquad(Mapbot[Mapfat[cur]], 1);
		}
		if (Maprig[cur] < 4) {
			sepaquad(Maprig[Mapfat[cur]], 1);
		}
		if (Maptop[cur] < 4) {
			sepaquad(Maptop[Mapfat[cur]], 1);
		}
		if (Maplef[cur] < 4) {
			sepaquad(Maplef[Mapfat[cur]], 1);
		}
		if (end > MapSize - 4) {
			return;
		}
		quaMap[cur] = end;
		end += 4;

		quaMap[quaMap[cur] + 0] = 0;
		quaMap[quaMap[cur] + 1] = 0;
		quaMap[quaMap[cur] + 2] = 0;
		quaMap[quaMap[cur] + 3] = 0;

		Mapfat[quaMap[cur] + 0] = cur;
		Mapfat[quaMap[cur] + 1] = cur;
		Mapfat[quaMap[cur] + 2] = cur;
		Mapfat[quaMap[cur] + 3] = cur;

		qual[quaMap[cur] + 0] = l / 2;
		qual[quaMap[cur] + 1] = l / 2;
		qual[quaMap[cur] + 2] = l / 2;
		qual[quaMap[cur] + 3] = l / 2;

		quau[quaMap[cur] + 0] = u;
		quau[quaMap[cur] + 1] = u + l / 2;
		quau[quaMap[cur] + 2] = u;
		quau[quaMap[cur] + 3] = u + l / 2;

		quav[quaMap[cur] + 0] = v;
		quav[quaMap[cur] + 1] = v;
		quav[quaMap[cur] + 2] = v + l / 2;
		quav[quaMap[cur] + 3] = v + l / 2;

		Mapbot[quaMap[cur] + 0] = quaMap[Mapbot[cur]] + 2;
		Mapbot[quaMap[cur] + 1] = quaMap[Mapbot[cur]] + 3;
		Mapbot[quaMap[cur] + 2] = quaMap[cur] + 0;
		Mapbot[quaMap[cur] + 3] = quaMap[cur] + 1;
		if (quaMap[Mapbot[cur]]) {
			Maptop[quaMap[Mapbot[cur]] + 2] = quaMap[cur] + 0;
			Maptop[quaMap[Mapbot[cur]] + 3] = quaMap[cur] + 1;
		}

		Maprig[quaMap[cur] + 0] = quaMap[cur] + 1;
		Maprig[quaMap[cur] + 1] = quaMap[Maprig[cur]] + 0;
		Maprig[quaMap[cur] + 2] = quaMap[cur] + 3;
		Maprig[quaMap[cur] + 3] = quaMap[Maprig[cur]] + 2;
		if (quaMap[Maprig[cur]]) {
			Maplef[quaMap[Maprig[cur]] + 0] = quaMap[cur] + 1;
			Maplef[quaMap[Maprig[cur]] + 2] = quaMap[cur] + 3;
		}

		Maptop[quaMap[cur] + 0] = quaMap[cur] + 2;
		Maptop[quaMap[cur] + 1] = quaMap[cur] + 3;
		Maptop[quaMap[cur] + 2] = quaMap[Maptop[cur]] + 0;
		Maptop[quaMap[cur] + 3] = quaMap[Maptop[cur]] + 1;
		if (quaMap[Maptop[cur]]) {
			Mapbot[quaMap[Maptop[cur]] + 0] = quaMap[cur] + 2;
			Mapbot[quaMap[Maptop[cur]] + 1] = quaMap[cur] + 3;
		}

		Maplef[quaMap[cur] + 0] = quaMap[Maplef[cur]] + 1;
		Maplef[quaMap[cur] + 1] = quaMap[cur] + 0;
		Maplef[quaMap[cur] + 2] = quaMap[Maplef[cur]] + 3;
		Maplef[quaMap[cur] + 3] = quaMap[cur] + 2;
		if (quaMap[Maplef[cur]]) {
			Maprig[quaMap[Maplef[cur]] + 1] = quaMap[cur] + 0;
			Maprig[quaMap[Maplef[cur]] + 3] = quaMap[cur] + 2;
		}
	}
}

void sep(void) {
	int i, j;
	for (i = 0; i < noqi; i++) {
		for (j = 0; j < noqi; j++) {
			quaMap[i + j * noqi + 4] = 0;
			Mapbot[i + j * noqi + 4] = (i + (j + noqi - 1) % noqi*noqi) + 4;
			Maprig[i + j * noqi + 4] = ((i + 1) % noqi + j * noqi) + 4;
			Maptop[i + j * noqi + 4] = (i + (j + 1) % noqi*noqi) + 4;
			Maplef[i + j * noqi + 4] = ((i + noqi - 1) % noqi + j * noqi) + 4;
			qual[i + j * noqi + 4] = 1.0 / noqi;
			quau[i + j * noqi + 4] = (double)i / noqi;
			quav[i + j * noqi + 4] = (double)j / noqi;
		}
	}
	end = noqi * noqi + 4;
	for (i = 4; i < end; i++) {
		sepaquad(i, 0);
	}
}

void draw(void) {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(0x00004100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(len * cos(ang1)*cos(ang2), len * sin(ang2), len * sin(ang1)*cos(ang2), 0, 0, 0, 0, cos(ang2), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glColorPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, vercount);

	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex3f(1.0f, 0.0f, -0.0f);
	glVertex3f(-1.0f, 0.0f, -0.0f);
	glVertex3f(0.0f, 1.0f, -0.0f);
	glVertex3f(0.0f, -1.0f, -0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glEnd();

	SwapBuffers(wglGetCurrentDC());
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PARASUR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PARASUR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PARASUR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PARASUR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	   return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_FILE_NEW:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, parainput);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break; 
	case WM_CREATE: {
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO,
			PFD_TYPE_RGBA,
			24,
			0,0,0,0,0,0,0,0,
			0,
			0,0,0,0,
			32,
			0,0,
			PFD_MAIN_PLANE,
			0,0,0,0
		};

		hdc1 = GetDC(hWnd);
		int uds = ::ChoosePixelFormat(hdc1, &pfd);
		::SetPixelFormat(hdc1, uds, &pfd);
		m_hrc = ::wglCreateContext(hdc1);
		::wglMakeCurrent(hdc1, m_hrc);
		glewInit();
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		versha = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(versha, 1, &shaderv, NULL);
		glCompileShader(versha);
		frasha = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frasha, 1, &shaderf, NULL);
		glCompileShader(frasha);
		pro = glCreateProgram();
		glAttachShader(pro, versha);
		glAttachShader(pro, frasha);
		glLinkProgram(pro);
		glUseProgram(pro);
		((bool(_stdcall*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(0);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &cbo);
		glGenBuffers(1, &tbo);
		glGenBuffers(1, &nbo);

		ang1 = 0;
		ang2 = 0;
		len = 2.5;
		
		/*float p[4] = { 0.9,0.9,3.0,0 };
		glLightfv(GL_LIGHT0, GL_POSITION, p);
		p[0] = 0.3; p[1] = 0.2; p[2] = 0.05; p[3] = 1.0;
		glLightfv(GL_LIGHT0, GL_AMBIENT, p);
		p[0] = 0.7; p[1] = 0.3; p[2] = -0.1; p[3] = 1.0;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, p);
		p[0] = 0; p[1] = 0.3; p[2] = 0.3; p[3] = 1.0;
		glLightfv(GL_LIGHT0, GL_SPECULAR, p);
		p[0] = 1; p[1] = 1; p[2] = 1;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, p);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, p);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, p);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 7);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);*/

		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, parainput);
		break;
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			draw();
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZE: {
		cx = lParam & 0xffff;
		cy = (lParam & 0xffff0000) >> 16;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-(float)cx / (cx + cy) / 50, (float)cx / (cx + cy) / 50, -(float)cy / (cx + cy) / 50, (float)cy / (cx + cy) / 50, 0.008, 100.0);
		glViewport(0, 0, cx, cy);
		draw();
		break;
	}
	case WM_MOUSEMOVE: {
		int x, y;
		x = (lParam & 0xffff);
		y = ((lParam & 0xffff0000) >> 16);
		if (MK_LBUTTON&wParam) {
			ang1 += (x - mx)*0.002;
			ang2 += (y - my)*0.002;
		}
		if (MK_RBUTTON&wParam) {
			len *= exp((my - y)*0.002);
		}
		mx = x;
		my = y;
		draw();
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK parainput(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDOK: {
			char x[1024], y[1024], z[1024], ul[1024], uh[1024], vl[1024], vh[1024];
			express c;
			/*HWND h1=GetDlgItem(hDlg, IDC_EDIT1);
			int i=GetWindowTextLength(h1);*/
			GetDlgItemText(hDlg, IDC_EDIT1, x, 1024);
			GetDlgItemText(hDlg, IDC_EDIT2, y, 1024);
			GetDlgItemText(hDlg, IDC_EDIT3, z, 1024);
			GetDlgItemText(hDlg, IDC_EDIT4, ul, 1024);
			GetDlgItemText(hDlg, IDC_EDIT5, uh, 1024);
			GetDlgItemText(hDlg, IDC_EDIT6, vl, 1024);
			GetDlgItemText(hDlg, IDC_EDIT7, vh, 1024); 
			if (getcoms(c, x, y, z)) {
				goto next;
			}
			if (!sscanf_s(ul, "%lf", &c.ul)) {
				goto next;
			}
			if(!sscanf_s(uh, "%lf", &c.uh)) {
				goto next;
			}
			if (!sscanf_s(vl, "%lf", &c.vl)) {
				goto next;
			}
			if (!sscanf_s(vh, "%lf", &c.vh)) {
				goto next;
			}
			free(parasur.cx);
			free(parasur.cy);
			free(parasur.cz);
			free(parasur.ox);
			free(parasur.oy);
			free(parasur.oz);
			parasur = c;
			sep();
			sprintf_s(x, "%d", end);
			MessageBox(hDlg, x, "end", MB_OK);
			scan();
		}
		case IDCANCEL:
		{
			next:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		}
		break;
	}
	}
	return (INT_PTR)FALSE;
}

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
extern "C" {
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}