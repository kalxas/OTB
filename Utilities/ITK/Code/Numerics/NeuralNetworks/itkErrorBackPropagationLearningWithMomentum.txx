/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkErrorBackPropagationLearningWithMomentum.txx,v $
  Language:  C++
  Date:      $Date: 2007-09-04 00:19:20 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkErrorBackPropagationLearningingWithMomentum_txx
#define __itkErrorBackPropagationLearningingWithMomentum_txx

#include "itkErrorBackPropagationLearningWithMomentum.h"
#include <fstream>


namespace itk
{
namespace Statistics
{

template<class LayerType, class TTargetVector>
ErrorBackPropagationLearningWithMomentum <LayerType,TTargetVector>
::ErrorBackPropagationLearningWithMomentum()
{
  m_Momentum = 0.9; //Default
}

template<class LayerType, class TTargetVector>
void
ErrorBackPropagationLearningWithMomentum<LayerType,TTargetVector>
::Learn(LayerInterfaceType * layer, ValueType lr)
{
  typedef typename LayerInterfaceType::WeightSetType::Pointer WeightSetPointer;
  WeightSetPointer outputweightset;
  WeightSetPointer inputweightset;
  outputweightset = layer->GetOutputWeightSet();
  inputweightset = layer->GetInputWeightSet();

  typedef typename LayerInterfaceType::ValuePointer InterfaceValuePointer;
  InterfaceValuePointer DWvalues_m_1 = inputweightset->GetPrevDWValues();
  InterfaceValuePointer DWvalues_m_2 = inputweightset->GetPrev_m_2DWValues();
  InterfaceValuePointer currentdeltavalues = inputweightset->GetTotalDeltaValues();
  InterfaceValuePointer DBValues = inputweightset->GetTotalDeltaBValues();
  InterfaceValuePointer PrevDBValues = inputweightset->GetPrevDBValues();

  int input_cols = inputweightset->GetNumberOfInputNodes();
  int input_rows = inputweightset->GetNumberOfOutputNodes();

  vnl_matrix<ValueType> DW_m_1(input_rows, input_cols);
  DW_m_1.fill(0);
  vnl_matrix<ValueType> DW_m_2(input_rows, input_cols);
  DW_m_2.fill(0);

  vnl_vector<ValueType> DB_temp;
  DB_temp.set_size(inputweightset->GetNumberOfOutputNodes());
  DB_temp.fill(0);
  vnl_vector<ValueType> DB;
  vnl_vector<ValueType> DB_m_1;
  DB.set_size(inputweightset->GetNumberOfOutputNodes());
  DB_m_1.set_size(inputweightset->GetNumberOfOutputNodes());
  DB.fill(0);
  DB_m_1.fill(0);
  DB.copy_in(DBValues);
  DB_m_1.copy_in(PrevDBValues);
  
  if (!inputweightset->GetFirstPass())
    {
    DW_m_1.copy_in(DWvalues_m_1);
    }
  if (!inputweightset->GetSecondPass())
    {
    DW_m_2.copy_in(DWvalues_m_2);
    }
  vnl_matrix<ValueType> DW_temp(currentdeltavalues,
                                           inputweightset->GetNumberOfOutputNodes(),
                                           inputweightset->GetNumberOfInputNodes());

  vnl_matrix<ValueType> DW_temp1(inputweightset->GetNumberOfOutputNodes(),
                                           inputweightset->GetNumberOfInputNodes());
  DW_temp1.fill(0);
  
  //Momentum
  if (!inputweightset->GetFirstPass())
    {
    DW_temp1 = (DW_temp * lr *(1 - m_Momentum)) + (DW_m_1 * m_Momentum);
    }
  else
    {
    DW_temp1 = DW_temp*lr;
    }
  DB_temp=(DB*lr);
  inputweightset->SetDWValues(DW_temp1.data_block());
  inputweightset->SetDBValues(DB_temp.data_block());
}

template<class LayerType, class TTargetVector>
void
ErrorBackPropagationLearningWithMomentum<LayerType,TTargetVector>
::Learn( LayerInterfaceType * itkNotUsed(layer), TTargetVector itkNotUsed(errors),ValueType itkNotUsed(lr))
{
  //It appears that this interface should not be called.
  //itkExceptionMacrto(<< "This should never be called");
}

/** Print the object */
template<class LayerType, class TTargetVector>
void  
ErrorBackPropagationLearningWithMomentum<LayerType,TTargetVector>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  os << indent << "ErrorBackPropagationLearningWithMomentum(" << this << ")" << std::endl; 
  os << indent << "m_Momentum = " << m_Momentum << std::endl;
  Superclass::PrintSelf( os, indent ); 
} 

} // end namespace Statistics
} // end namespace itk

#endif
