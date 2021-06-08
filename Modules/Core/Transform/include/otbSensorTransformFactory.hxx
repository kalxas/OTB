/*
 * Copyright (C) 2005-2021 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef otbSensorTransformFactory_hxx
#define otbSensorTransformFactory_hxx

#include "otbSensorTransformFactory.h"
#include "otbRPCTransformBase.h"
#include "otbRPCForwardTransformFactory.h"
#include "otbRPCInverseTransformFactory.h"
#include "itkMutexLockHolder.h"

namespace otb
{

template <class TScalarType, unsigned int NInputDimensions, unsigned int NOutputDimensions>
itk::SimpleMutexLock SensorTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::m_mutex;

template <class TScalarType, unsigned int NInputDimensions, unsigned int NOutputDimensions>
typename SensorTransformBase<TScalarType, NInputDimensions,NOutputDimensions>::Pointer
SensorTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::CreateTransform(const ImageMetadata &imd,TransformDirection direction)
{
  RegisterBuiltInFactories();

  auto possibleobjects = itk::ObjectFactoryBase::CreateAllInstance("otbRPCTransformBase");
  //TODO: add a list of possible SAR objects and concatenate rpc and sar list in one main list
  for (auto && po : possibleobjects)
  {
    SensorTransformBase<TScalarType, NInputDimensions,NOutputDimensions>* io = dynamic_cast<SensorTransformBase<TScalarType, NInputDimensions,NOutputDimensions>*>(po.GetPointer());
    if (io)
    {
        io->SetMetadata(imd);
        if (io->IsValidSensorModel() && io->getDirection() == direction)
        {
          return io;
        }
    }
    //io is null is possible because createAllInstance will return all known instances with different types like SensorTransform<double,2,2> SensorTransform<double,3,3> which has been registered before
  }
  // if no object has been found, we have to warn the user
  otbLogMacro(Warning, << "The SensorTransform factory could not find a compatible Sensor Transform");
  return nullptr;
}

template <class TScalarType, unsigned int NInputDimensions, unsigned int NOutputDimensions>
void SensorTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::DoRegisterBuiltInFactories()
{
  itk::ObjectFactoryBase::RegisterFactory(RPCForwardTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::New());
  itk::ObjectFactoryBase::RegisterFactory(RPCInverseTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::New());
}


template <class TScalarType, unsigned int NInputDimensions, unsigned int NOutputDimensions>
void SensorTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::RegisterBuiltInFactories()
{
  static std::once_flag reg_flag;
  std::call_once(reg_flag, &SensorTransformFactory<TScalarType, NInputDimensions, NOutputDimensions>::DoRegisterBuiltInFactories);
}

template <class TScalarType, unsigned int NInputDimensions, unsigned int NOutputDimensions>
void SensorTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::RegisterFactory(itk::ObjectFactoryBase* factory)
{
  // Unregister any previously registered factory of the same class
  // Might be more intensive but static bool is not an option due to
  // ld error.
  itk::ObjectFactoryBase::UnRegisterFactory(factory);
  itk::ObjectFactoryBase::RegisterFactory(factory);
}

template <class TScalarType, unsigned int NInputDimensions, unsigned int NOutputDimensions>
void SensorTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>::CleanFactories()
{
  itk::MutexLockHolder<itk::SimpleMutexLock> lockHolder(m_mutex);

  std::list<itk::ObjectFactoryBase*>           factories = itk::ObjectFactoryBase::GetRegisteredFactories();
  std::list<itk::ObjectFactoryBase*>::iterator itFac;

  for (itFac = factories.begin(); itFac != factories.end(); ++itFac)
  {
    RPCForwardTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>* rpcForwardFactory =
      dynamic_cast<RPCForwardTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>*>(*itFac);
    if (rpcForwardFactory)
    {
      itk::ObjectFactoryBase::UnRegisterFactory(rpcForwardFactory);
      continue;
    }

    RPCInverseTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>* rpcInverseFactory =
      dynamic_cast<RPCInverseTransformFactory<TScalarType, NInputDimensions,NOutputDimensions>*>(*itFac);
    if (rpcInverseFactory)
    {
      itk::ObjectFactoryBase::UnRegisterFactory(rpcInverseFactory);
      continue;
    }
  }
}

} // end namespace otb

#endif
