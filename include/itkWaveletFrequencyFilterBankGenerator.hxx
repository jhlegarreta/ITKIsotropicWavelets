/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkWaveletFrequencyFilterBankGenerator_hxx
#define itkWaveletFrequencyFilterBankGenerator_hxx
#include "itkWaveletFrequencyFilterBankGenerator.h"
#include "itkFrequencyShrinkImageFilter.h"
#include "itkNumericTraits.h"

namespace itk {
template < typename TOutputImage, typename TWaveletFunction, typename TFrequencyRegionIterator>
WaveletFrequencyFilterBankGenerator<TOutputImage, TWaveletFunction, TFrequencyRegionIterator>
::WaveletFrequencyFilterBankGenerator()
  : Superclass(),
    m_InverseBank(false)
{
   this->m_Evaluator = TWaveletFunction::New();
}

template < typename TOutputImage, typename TWaveletFunction, typename TFrequencyRegionIterator>
void WaveletFrequencyFilterBankGenerator<TOutputImage, TWaveletFunction, TFrequencyRegionIterator>
::SetInverseBankOn()
{
  this->SetInverseBank(true);
}

template < typename TOutputImage, typename TWaveletFunction, typename TFrequencyRegionIterator>
void WaveletFrequencyFilterBankGenerator<TOutputImage, TWaveletFunction, TFrequencyRegionIterator>
::PrintSelf(std::ostream &os, Indent indent) const {
  Superclass::PrintSelf(os, indent);

  os << indent << "InverseBank: " << (this->m_InverseBank ? "true":"false")
     << std::endl;
}

template < typename TOutputImage, typename TWaveletFunction, typename TFrequencyRegionIterator>
void WaveletFrequencyFilterBankGenerator<TOutputImage, TWaveletFunction, TFrequencyRegionIterator>
::GenerateData()
{
  this->m_Evaluator->SetHighPassSubBands(this->GetHighPassSubBands());
  this->AllocateOutputs();

  for (unsigned int level = 0; level < this->GetLevels(); ++level)
    {
    // TODO: It to calculate frequency modulo only once per level (optimization)
    for (unsigned int band = 0; band < this->GetHighPassSubBands() + 1; ++band)
      {
      if (band == 0 && level != this->GetLevels() - 1)
        continue;
      unsigned int current_output_index = level * this->GetHighPassSubBands() + band;
      OutputRegionIterator frequencyIt(this->GetOutput(current_output_index), this->GetOutput(current_output_index)->GetRequestedRegion());
      for (frequencyIt.GoToBegin(); !frequencyIt.IsAtEnd(); ++frequencyIt)
        {
        FunctionValueType w(0);
        w = static_cast<FunctionValueType>(
          sqrt(frequencyIt.GetFrequencyModuloSquare()));

        // band = 0 is low pass filter, band = GetHighPassSubBands() is high-pass filter.
        FunctionValueType evaluatedSubBand = this->m_InverseBank ?
          this->m_Evaluator->EvaluateInverseSubBand(w , band) :
          this->m_Evaluator->EvaluateForwardSubBand(w , band);

        frequencyIt.Set(
          itk::NumericTraits<typename OutputImageType::PixelType>::Zero +
          static_cast<typename OutputImageType::PixelType::value_type>(evaluatedSubBand));
        ++frequencyIt;
        }
      }
    }
}
}  // end namespace itk
#endif
