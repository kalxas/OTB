/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkOneHiddenLayerBackPropagationNeuralNetwork.h,v $
Language:  C++
Date:      $Date: 2007-08-17 13:10:57 $
Version:   $Revision: 1.7 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOneHiddenLayerBackPropNeuralNetworkBase_h
#define __itkOneHiddenLayerBackPropNeuralNetworkBase_h

#include "itkMultilayerNeuralNetworkBase.h"
#include "itkBackPropagationLayer.h"
#include "itkSigmoidTransferFunction.h"
#include "itkLogSigmoidTransferFunction.h"
#include "itkTanSigmoidTransferFunction.h"
#include "itkHardLimitTransferFunction.h"
#include "itkSignedHardLimitTransferFunction.h"
#include "itkGaussianTransferFunction.h"
#include "itkIdentityTransferFunction.h"
#include "itkSumInputFunction.h"

//#include "itkProductInputFunction.h"

namespace itk
{
  namespace Statistics
    {

    template<class TMeasurementVector, class TTargetVector>
      class OneHiddenLayerBackPropagationNeuralNetwork :
      public MultilayerNeuralNetworkBase<TMeasurementVector, TTargetVector, BackPropagationLayer<TMeasurementVector, TTargetVector> >
        {
      public:
        typedef OneHiddenLayerBackPropagationNeuralNetwork Self;
        typedef MultilayerNeuralNetworkBase<TMeasurementVector, TTargetVector, BackPropagationLayer<TMeasurementVector, TTargetVector> > Superclass;
        typedef SmartPointer<Self> Pointer;
        typedef SmartPointer<const Self> ConstPointer;

        typedef typename Superclass::ValueType ValueType;
        typedef typename Superclass::MeasurementVectorType MeasurementVectorType;
        typedef typename Superclass::TargetVectorType TargetVectorType;
        typedef typename Superclass::NetworkOutputType NetworkOutputType;

        typedef typename Superclass::LayerInterfaceType LayerInterfaceType;
        typedef typename Superclass::LearningLayerType LearningLayerType;

        typedef typename Superclass::WeightVectorType WeightVectorType;
        typedef typename Superclass::LayerVectorType LayerVectorType;

        typedef typename Superclass::TransferFunctionInterfaceType TransferFunctionInterfaceType;
        typedef typename Superclass::InputFunctionInterfaceType InputFunctionInterfaceType;

        /* Method for creation through the object factory. */
        itkTypeMacro(OneHiddenLayerBackPropagationNeuralNetwork,
          MultilayerNeuralNetworkBase);
        itkNewMacro(Self) ;

        //Add the layers to the network.
        // 1 input, 1 hidden, 1 output
        void Initialize();

        itkSetMacro(NumOfInputNodes, unsigned int);
        itkGetConstReferenceMacro(NumOfInputNodes, unsigned int);

        itkSetMacro(NumOfFirstHiddenNodes, unsigned int);
        itkGetConstReferenceMacro(NumOfFirstHiddenNodes, unsigned int);

        itkSetMacro(NumOfOutputNodes, unsigned int);
        itkGetConstReferenceMacro(NumOfOutputNodes, unsigned int);

        itkSetMacro(FirstHiddenLayerBias, ValueType);
        itkGetConstReferenceMacro(FirstHiddenLayerBias, ValueType);

//#define __USE_OLD_INTERFACE  Comment out to ensure that new interface works
#ifdef __USE_OLD_INTERFACE
        //Original Function name before consistency naming changes
        inline void SetNumOfHiddenNodes(const unsigned int & x) { SetNumOfFirstHiddenNodes(x); }
        inline unsigned int GetNumOfHiddenNodes(void) const { return GetNumOfFirstHiddenNodes(); }
        inline void SetHiddenLayerBias(const ValueType & bias) { SetFirstHiddenLayerBias(bias); }
        ValueType GetHiddenLayerBias(void) const { return GetFirstHiddenLayerBias();};
#endif
        itkSetMacro(OutputLayerBias, ValueType);
        itkGetConstReferenceMacro(OutputLayerBias, ValueType);

        virtual NetworkOutputType GenerateOutput(TMeasurementVector samplevector);

        void SetInputFunction(InputFunctionInterfaceType* f);
        void SetInputTransferFunction(TransferFunctionInterfaceType* f);
#ifdef __USE_OLD_INTERFACE
        //Original Function name before consistency naming changes
        inline void SetHiddenTransferFunction(TransferFunctionInterfaceType* f) { SetFirstHiddenTransferFunction (f); };
#endif
        void SetFirstHiddenTransferFunction(TransferFunctionInterfaceType* f);
        void SetOutputTransferFunction(TransferFunctionInterfaceType* f);
      protected:

        OneHiddenLayerBackPropagationNeuralNetwork();
        virtual ~OneHiddenLayerBackPropagationNeuralNetwork(){};

        /** Method to print the object. */
        virtual void PrintSelf( std::ostream& os, Indent indent ) const;

      private:

        unsigned int m_NumOfInputNodes;
        unsigned int m_NumOfFirstHiddenNodes;
        unsigned int m_NumOfOutputNodes;

        ValueType m_FirstHiddenLayerBias;
        ValueType m_OutputLayerBias;

        typename InputFunctionInterfaceType::Pointer    m_InputFunction;
        typename TransferFunctionInterfaceType::Pointer m_InputTransferFunction;
        typename TransferFunctionInterfaceType::Pointer m_FirstHiddenTransferFunction;
        typename TransferFunctionInterfaceType::Pointer m_OutputTransferFunction;
        };

    } // end namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOneHiddenLayerBackPropagationNeuralNetwork.txx"
#endif

#endif
