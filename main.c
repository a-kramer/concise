#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//const double log10=2.302585;
//const double log2=0.6931472;

enum exponent_format {plain,parentheses, brackets, braces, mini};
enum out_format {eE, asterisk, latex, cross, dot, table};
const char *superscript[]={"⁰","¹","²","³","⁴","⁵","⁶","⁷","⁸","⁹",".",NULL};

int numeric(char c){
	if ('0' <= c && c <= '9') return 1;
	else if (c=='.' || c=='+' || c=='-') return 1;
	else return 0;
}

/* numbe a = v ± u */
struct num {
	union{ /* the estimate of something */
		double v;
		double value;
		double expectedValue;
		double mean;
	};
	union { /* the uncertainty of the estimate */
		double u;
		double error;
		double stdDev;
		double uncertainty;
	};
};


/* 5 = [101]; a=b^5 = b^[101] = b^(1*2^2 + 0*2^1 + 1*2^0) = b^(2^2) * b^(2^0) */
double mpowl(double b, int n){
	if (n==0) return 1.0;
	int m=n>0 ? n : -n;
	double a=1.0;
	while (m){
		if (m&1) a*=b;
		b*=b;
		m>>=1;
	}
	return n>0 ? a : 1.0/a;
}

void print_concise(struct num x, enum out_format OF, enum exponent_format EF){
	int vscale=round(log10(fabs(x.value)));
	int uscale=floor(log10(fabs(x.error)));
	int d=vscale-uscale+1;
	double v=x.value*pow(10,-vscale);
	int u=x.error*pow(10,1-uscale);
	if (u%10==0 && d>0){
		d--;
		u/=10;
	}
	if (d<0) {
		v*=pow(10,d);
		u*=pow(10,d);
		vscale+=1-d;
		d=0;
	}
	//printf("v=%f (%i); u=%f (%i); %i; ",x.v,vscale,x.u,uscale,d);
	if (vscale){
		printf("%.*f(%i)",d,v,u);
		switch(OF){
		case cross: printf("×10"); break;
		case dot: printf("·10"); break;
		case latex: printf("\\times 10"); break;
		case asterisk: printf("*10"); break;
		case eE: printf("E"); break;
		default: printf("E");
		}
		switch(EF){
		case parentheses: printf("^(%i)\n",vscale); break;
		case brackets: printf("^[%i]\n",vscale); break;
		case braces: printf("^{%i}\n",vscale); break;
		case mini: printf("%s%s%s\n",(vscale>0?"⁺":"⁻"),superscript[abs(vscale)/10],superscript[abs(vscale)%10]); break;
		default: printf("%i\n",vscale);
		}
	} else {
		printf("%.*f(%i)\n",d,v,u);
	}
}

void print_number(struct num x, enum out_format OF, enum exponent_format EF){
	if (OF==table){
		printf("%g\t%g\n",x.v,x.u);
		return;
	}
	int vscale=round(log10(fabs(x.value)));
	int uscale=floor(log10(fabs(x.error)));
	int d=vscale-uscale+1;
	double v=x.value*pow(10,-vscale);
	double u=x.error*pow(10,-vscale);
	d-=((int)round(x.error*pow(10,1-uscale))%10)==0;
	if (vscale){
		if (OF==latex){
			printf("(%.*f\\pm %.*f)",d,v,d,u);
		} else {
			printf("(%.*f±%.*f)",d,v,d,u);
		}
		switch (OF){
		case cross: printf("×10"); break;
		case dot: printf("·10"); break;
		case latex: printf("\\times 10"); break;
		case asterisk: printf("*10"); break;
		case eE: printf("E"); break;
		default: printf("E");
		}
		switch(EF){
		case parentheses: printf("^(%i)\n",vscale); break;
		case brackets: printf("^[%i]\n",vscale); break;
		case braces: printf("^{%i}\n",vscale); break;
		case mini: printf("%s%s%s\n",(vscale>0?"⁺":"⁻"),superscript[abs(vscale)/10],superscript[abs(vscale)%10]); break;
		default: printf("%i\n",vscale);
		}
	} else {
		if (OF==latex){
			printf("%.*f\\pm %.*f",d,v,d,u);
		} else {
			printf("%.*f±%.*f",d,v,d,u);
		}
	}
}

/* this will only read: 12.34(56)E-7, like this, with an E*/
struct num read_concise(char *line){
	struct num a;
	char *ptr=line;
	char *ptr_u=line, *ptr_e=line;
	int vscale=0;
	double v=strtod(line,&ptr_u);
	int u=0;
	//int ulen=2;
	char *decimal=strchr(line,'.');
	int digits=ptr_u-(decimal?decimal:line)-1;
	while (ptr_u && *ptr_u && !numeric(*ptr_u)) ptr_u++;
	if (numeric(*ptr_u)) {
		u=strtol(ptr_u,&ptr_e,10);
		//ulen=ptr_e-ptr_u;
	}
	while (ptr_e && *ptr_e && !numeric(*ptr_e)) ptr_e++;
	if (numeric(*ptr_e)) vscale=strtol(ptr_e,NULL,10);
	a.v=v*pow(10,vscale);
	a.u=u*pow(10,-digits+vscale);
	return a;
}

struct num read_number(char *line){
	struct num a;
	char *ptr=line;
	a.value = strtod(line,&ptr);
	char *decimal = strchr(line,'.');
	int digits = decimal?(ptr-decimal-1):0;
	while (ptr && *ptr && !numeric(*ptr)) ptr++;
	if (ptr && *ptr){
		a.error = strtod(ptr,NULL);
	} else {
		a.error = pow(10,-digits);
	}
	return a;
}

int main(int argc, char *argv[]){
	size_t n=32;
	char *line=malloc(n);
	ssize_t m;
	int j;
	struct num a;
	enum out_format OF=table;
	enum exponent_format EF=plain;
	for (j=1;j<argc;j++){
		if (0==strcmp("--asterisk",argv[j]) || 0==strcmp("--star",argv[j]) || 0==strcmp("-*",argv[j])) {
			OF=asterisk;
			EF=parentheses;
		} else if (0==strcmp("--latex",argv[j]) || 0==strcmp("--LaTeX",argv[j]) || 0==strcmp("-\\",argv[j])) {
			OF=latex;
			EF=braces;
		} else if (0==strcmp("--cross",argv[j]) || 0==strcmp("--times",argv[j]) || 0==strcmp("-x",argv[j])) {
			OF=cross;
			EF=braces;
		} else if (0==strcmp("--dot",argv[j]) || 0==strcmp("--cdot",argv[j]) || 0==strcmp("-.",argv[j])) {
			OF=dot;
			EF=mini;
		} else if (0==strcmp("--plain",argv[j]) || 0==strcmp("-E",argv[j])) {
			OF=eE;
			EF=plain;
		} else if (0==strcmp("--parentheses",argv[j]) || 0==strcmp("--paren",argv[j]) || 0==strcmp("-(",argv[j])) {
			EF=parentheses;
		} else if (0==strcmp("--braces",argv[j]) || 0==strcmp("--brace",argv[j]) || 0==strcmp("-{",argv[j])) {
			EF=braces;
		} else if (0==strcmp("--brackets",argv[j]) || 0==strcmp("--bracket",argv[j]) || 0==strcmp("-[",argv[j])) {
			EF=brackets;
		} else if (0==strcmp("--mini",argv[j]) || 0==strcmp("--superscript",argv[j]) || 0==strcmp("-^",argv[j])) {
			EF=mini;
		} else {
			fprintf(stderr,"[%s] unknown option: %s",__func__,argv[j]);
		}
	}
	while ((m=getline(&line,&n,stdin))>0 && !feof(stdin)){
		if (strchr(line,'(')){
			a=read_concise(line);
			print_number(a,OF,EF);
		} else {
			a=read_number(line);
			print_concise(a,OF,EF);
		}
	}
	return EXIT_SUCCESS;
}
