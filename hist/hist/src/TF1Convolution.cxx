//
//  TF1Convolution.cpp
//  
//
//  Created by Aurélie Flandi on 27.08.14.
//
//

#include "TF1Convolution.h"
#include "Riostream.h"
#include "TROOT.h"
#include "TMath.h"
#include "Math/Integrator.h"
#include "Math/IntegratorMultiDim.h"
#include "Math/IntegratorOptions.h"
#include "Math/GaussIntegrator.h"
#include "Math/GaussLegendreIntegrator.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "Math/Functor.h"

//ClassImp(TF1Convolution)


// class wrapping evaluation of TF1(t) * TF1(x-t)
class TF1Convolution_EvalWrapper
{
   std::shared_ptr < TF1 > fFunction1;
   std::shared_ptr < TF1 > fFunction2;
   Double_t fT0;
   
public:

   TF1Convolution_EvalWrapper(const TF1* function1 , const TF1* function2, Double_t t)
   :fT0(t)
   {
      std::shared_ptr < TF1 > f1((TF1*)function1->Clone());
      std::shared_ptr < TF1 > f2((TF1*)function2->Clone());
      fFunction1 = f1;
      fFunction2 = f2;
   }
   Double_t operator()(Double_t x) const
   {
      return fFunction1->Eval(x) * fFunction2->Eval(x-fT0);
   }
   
   
};


TF1Convolution::TF1Convolution(TF1* function1, TF1* function2)
{
   std::shared_ptr < TF1 > f1((TF1*)function1->Clone());
   std::shared_ptr < TF1 > f2((TF1*)function2->Clone());
   
   fFunction1  = f1;
   fFunction2  = f2;
   fXmin       = fFunction1->GetXmin();
   fXmax       = fFunction1->GetXmax();
   fNofParams1 = f1->GetNpar();
   fNofParams2 = f2->GetNpar();
   fParams1    = std::vector<Double_t>(fNofParams1);
   fParams2    = std::vector<Double_t>(fNofParams2);
   
   for (int i=0; i<fNofParams1; i++)
   {
      fParams1[i] = fFunction1->GetParameter(i);
   }
   for (int i=0; i<fNofParams2; i++)
   {
      fParams2[i] = fFunction2->GetParameter(i);
   }
   Int_t indexcst = fFunction1-> GetParNumber("Constant");
   if (indexcst!=-1)
   {
      fFunction1->FixParameter(indexcst,1.);
      fNofParams1=fNofParams1-1;
      fParams1.erase(fParams1.begin()+indexcst);
   }
}
/*
TF1Convolution::TF1Convolution(TF1* function1, TF1* function2, Double_t xmin, Double_t xmax)
{
   std::shared_ptr < TF1 > f1((TF1*)function1->Clone());
   std::shared_ptr < TF1 > f2((TF1*)function2->Clone());
   
   fFunction1 = f1;
   fFunction2 = f2;
   fXmin      = xmin;
   fXmax      = xmax;

}
*/
Double_t TF1Convolution::MakeConvolution(Double_t t)
{
   TF1Convolution_EvalWrapper fconv((TF1*)fFunction1->Clone(), (TF1*)fFunction2->Clone(), t);
   Double_t result = 0;
   
   if (ROOT::Math::IntegratorOneDimOptions::DefaultIntegratorType() == ROOT::Math::IntegrationOneDim::kGAUSS )
   {
      ROOT::Math::GaussIntegrator integrator(1e-9, 1e-9);
      integrator.SetFunction(ROOT::Math::Functor1D(fconv));
      if      (fXmin != - TMath::Infinity() && fXmax != TMath::Infinity() )
         result =  integrator.Integral(fXmin, fXmax);
      else if (fXmin == - TMath::Infinity() && fXmax != TMath::Infinity() )
         result = integrator.IntegralLow(fXmax);
      else if (fXmin != - TMath::Infinity() && fXmax == TMath::Infinity() )
         result = integrator.IntegralUp(fXmin);
      else if (fXmin == - TMath::Infinity() && fXmax == TMath::Infinity() )
         result = integrator.Integral();
      //error = integrator.Error();
   }
   else {
      ROOT::Math::IntegratorOneDim integrator(fconv, ROOT::Math::IntegratorOneDimOptions::DefaultIntegratorType(), 1e-9, 1e-9);
      if (fXmin != - TMath::Infinity() && fXmax != TMath::Infinity() )
         result =  integrator.Integral(fXmin, fXmax);
      else if (fXmin == - TMath::Infinity() && fXmax != TMath::Infinity() )
         result = integrator.IntegralLow(fXmax);
      else if (fXmin != - TMath::Infinity() && fXmax == TMath::Infinity() )
         result = integrator.IntegralUp(fXmin);
      else if (fXmin == - TMath::Infinity() && fXmax == TMath::Infinity() )
         result = integrator.Integral();
      //error = iod.Error();
   }
   return result;
}

Double_t TF1Convolution::operator()(Double_t* t, Double_t* p)//used in TF1 when doing the fit, will be valuated at each point
{
   if (p!=0)   TF1Convolution::SetParameters(p);                           // first refresh the parameters
   
   return MakeConvolution(t[0]);
}

void TF1Convolution::SetParameters(Double_t* p)
{
   for (int i=0; i<fNofParams1; i++)
   {
      fFunction1 -> SetParameter(i,p[i]);
   }
   Int_t k = 0;
   for (int i=fNofParams1; i<fNofParams1+fNofParams2; i++)
   {
      fFunction2->SetParameter(k,p[i]);
      k++;
   }
}

void TF1Convolution::SetParameters(Double_t p0, Double_t p1, Double_t p2, Double_t p3,
                                   Double_t p4, Double_t p5, Double_t p6, Double_t p7)
{
   Double_t params[]={p0,p1,p2,p3,p4,p5,p6,p7};
   TF1Convolution::SetParameters(params);


}
