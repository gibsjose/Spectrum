//************************************************************/
//
//	Draw Utilities Implementation
//
//	Some useful graph utilities for Spectrum
//
//	@Author: 	J. Gibson, C. Embree, T. Carli - CERN
//	@Date:		20.11.2014
//	@Email:		gibsjose@mail.gvsu.edu
//
//************************************************************/

#include "SPXDrawUtilities.h"

const std::string cn = "SPXDrawUtilities::";

/*
void myText(Double_t x,Double_t y,Color_t color,char *text,Double_t tsize=0.06) {
  // plot a Text
  //
  //Double_t tsize=0.05;
  TLatex l; //l.SetTextAlign(12); 
  l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x,y,text);
}
*/ 


void SPXDrawUtilities::BoxText(double x, double y, double xboxsize, double yboxsize, int mcolor, std::string text, int lcol, int lstyle, double tsize) {
 // plot a Box and a Text
 //
 std::string mn = "BoxText: ";

 const bool debug=false;

 TLatex l; l.SetTextAlign(12); //l.SetTextSize(tsize); 
 l.SetNDC(); l.SetTextSize(tsize);
 if (text.size()>0)
  l.DrawLatex(x,y,TString(text));

 double y1=y-yboxsize/2.;
 double y2=y+yboxsize/2.;
 double x2=x-0.3*tsize;
 double x1=x2-xboxsize;

 if (debug) printf("%s x1= %f x2= %f y1= %f y2= %f \n",mn.c_str(),x1,x2,y1,y2);

 TPave *mbox= new TPave(x1,y1,x2,y2,0,"NDC");

 mbox->SetFillColor(mcolor);
 mbox->SetFillStyle(1001);
 mbox->Draw();

 TLine mline;
 mline.SetLineWidth(2);
 mline.SetLineColor(lcol);
 mline.SetLineStyle(lstyle);
 //Double_t ym=(y1+y2)/2.;
 //mline.DrawLineNDC(x1,y,x2,ym);

}

/*
void myMarkerText(Double_t x,Double_t y,Int_t color,Int_t mstyle,char *text, Double_t tsize=0.05) {
  // plot a Marker and a Text
  //
  //  printf("**myMarker: text= %s\ m ",text);

  TMarker *marker = new TMarker(x-(0.6*tsize),y,8);
  marker->SetMarkerColor(color);  marker->SetNDC();
  marker->SetMarkerStyle(mstyle);
  //marker->SetMarkerSize(2.0);
  marker->Draw();

  TLatex l; l.SetTextAlign(12); //l.SetTextSize(tsize); 
  l.SetNDC(); l.SetTextSize(tsize);
  l.DrawLatex(x,y,text);
  //cout<<" text= "<<text<<endl;
}
void myLineText(Double_t x,Double_t y,Int_t mcolor,Int_t mstyle,char *text, Double_t tsize=0.05) {
  // plot a line and a Text
  //
  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize); l.SetNDC(); 
  l.DrawLatex(x,y,text); 

  Double_t x1=x-1.3*tsize;
  Double_t x2=x-0.3*tsize;
 
  if (tsize<0.05) {
   x1=x-2.5*tsize;
   x2=x-0.3*tsize;

  }

  //y+=0.5*tsize;

  TLine mline;
  // mline.SetBit(TLine::kLineNDC );  // will set the bit kTRUE
  //mline.SetBit(TLine::kLineNDC,kFALSE);
  mline.SetLineWidth(2);
  mline.SetLineColor(mcolor);
  mline.SetLineStyle(mstyle);
  mline.DrawLineNDC(x1,y,x2,y);
  // mline->Draw();
}

void myLineText(Double_t x,Double_t y,Int_t mcolor,Int_t mstyle,char *text, 
                Double_t tsize, Double_t lsize ) {
  // plot a line and a Text
  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize); l.SetNDC(); 
  l.DrawLatex(x,y,text); 

  Double_t x1=x-1.5*lsize;
  Double_t x2=x-0.5*lsize;
 
  //  y+=tsize;

  TLine mline;
  // mline.SetBit(TLine::kLineNDC );  // will set the bit kTRUE
  //mline.SetBit(TLine::kLineNDC,kFALSE);
  mline.SetLineWidth(2);
  mline.SetLineColor(mcolor);
  mline.SetLineStyle(mstyle);
  mline.DrawLineNDC(x1,y,x2,y);
  // mline->Draw();
}



void myline(Double_t lmax, Double_t xpercent, Double_t offset=1., Double_t tsize=0.06) {

 const char help[]={"%"};

 TLine *mline = new TLine(0,offset,lmax,offset);
 mline->SetLineColor(1);
 mline->SetLineStyle(2);
 mline->Draw();
 TLine *mlineup = new TLine(0,offset+xpercent,lmax,offset+xpercent);
 mlineup->SetLineColor(1);
 mlineup->SetLineStyle(2);
 mlineup->Draw();
 TLine *mlinedn = new TLine(0,offset-xpercent,lmax,offset-xpercent);
 mlinedn->SetLineColor(1);
 mlinedn->SetLineStyle(2);
 mlinedn->Draw();
 //lmax+=2.;
 lmax*=1.01;
 TArrow *arrow=new TArrow(lmax,offset+xpercent,lmax,offset-xpercent,0.02,"<>");
 arrow->SetLineColor(1);
 arrow->SetLineWidth(1);
 arrow->SetLineStyle(1);
 arrow->Draw();

 TLatex l; l.SetTextAlign(12); l.SetTextSize(1.2*tsize);
 l.SetTextColor(1);
 char arg[100]; 
 //printf("xpercent*1000= %f \n",xpercent*1000);
 // printf("xpercent*1000= %d \n",xpercent*1000);
 if (xpercent*1000.<1){
  char *help=(char*) {(char*) "\275"};
  sprintf(arg,"#pm %3.1f%s",xpercent*1000,help);
} else {
  //printf(" xpercent= %f \n",xpercent);
  sprintf(arg," #pm %3.1f%s ",xpercent*100,help);
 }
 //l.DrawLatex(lmax+0.3,1.,arg);   
 l.DrawLatex(lmax*1.01,offset,arg);   

}

void myline2(Double_t lmin, Double_t lmax, Double_t xpercent, Double_t offset=1., Double_t tsize=0.06) {


 const char help[]={"%"};

 TLine *mline = new TLine(lmin,offset,lmax,offset);
 mline->SetLineColor(1);
 mline->SetLineStyle(2);
 mline->Draw();
 TLine *mlineup = new TLine(lmin,offset+xpercent,lmax,offset+xpercent);
 mlineup->SetLineColor(1);
 mlineup->SetLineStyle(2);
 mlineup->Draw();
 TLine *mlinedn = new TLine(lmin,offset-xpercent,lmax,offset-xpercent);
 mlinedn->SetLineColor(1);
 mlinedn->SetLineStyle(2);
 mlinedn->Draw();
 //lmax+=2.;
 lmax*=1.01;
 TArrow *arrow=new TArrow(lmax,offset+xpercent,lmax,offset-xpercent,0.02,"<>");
 arrow->SetLineColor(1);
 arrow->SetLineWidth(1);
 arrow->SetLineStyle(1);
 arrow->Draw();
TLatex l; l.SetTextAlign(12); l.SetTextSize(1.2*tsize);
 l.SetTextColor(1);
 char arg[100]; 
 //printf("xpercent*1000= %f \n",xpercent*1000);
 // printf("xpercent*1000= %d \n",xpercent*1000);
 if (xpercent*1000.<1){
  char *help=(char*) {(char*) "\275"};
  sprintf(arg,"#pm %3.1f%s",xpercent*1000,help);
} else {
  //printf(" xpercent= %f \n",xpercent);
  //sprintf(arg," #pm %3.1f%s ",xpercent*100,help);
  sprintf(arg," #pm %2.0f%s ",xpercent*100,help);
 }
 //l.DrawLatex(lmax+0.3,1.,arg);   
 l.DrawLatex(lmax*1.01,offset,arg);   

}
*/
TString SPXDrawUtilities::FormatwithExp(double xinput){
 TString datalabel="";
 int exp; double x;

 if (xinput==0) return TString("");

 SPXMathUtilities::frexp10(xinput, exp, x);
 //if (debug) std::cout<<cn<<"FormatwithExp: value= "<<xinput<<" x= "<<x<<" 10^"<<exp<<std::endl;
 datalabel+=Form("%1.1f 10^{%d}",x,exp);
 return datalabel;
}
