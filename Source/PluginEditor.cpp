/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusFlangerAudioProcessorEditor::ChorusFlangerAudioProcessorEditor (ChorusFlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    auto& params = processor.getParameters(); //get the list of parameters we can use
    
    /*This bit is is the Dry Wet Slider*/
    
    AudioParameterFloat* dryWetParameter = (AudioParameterFloat*)params.getUnchecked(0); //set to 0 because it is the 1st parameter - indexing from 0
    
    mDryWetSlider.setBounds(0, 0, 100, 100); //sets position and size
    mDryWetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag); //sets style
    mDryWetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0); //gets rid of the slider text box from the UI
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end); //sets the range
    mDryWetSlider.setValue(*dryWetParameter); //sets the slider value to the current value of the dry wet parameter in the processor
    addAndMakeVisible(mDryWetSlider); //makes the slider visible on the screen

    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); }; //lambda function to update slider when the parameter value is changed
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); }; //lambda for what happens when you start dragging the slider
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); }; //lambda for what happens when you stop moving the slider
    
    /*This bit is the Feedback Slider*/
    
    AudioParameterFloat* mFeedbackParameter = (AudioParameterFloat*)params.getUnchecked(4); //set to 1 because it is the 2nd parameter - indexing from 0
    
    mFeedbackSlider.setBounds(100, 0, 100, 100); //sets the position and size
    mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag); //sets style
    mFeedbackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0); //gets rid of the slider text box from the UI
    mFeedbackSlider.setRange(mFeedbackParameter->range.start, mFeedbackParameter->range.end); //sets the range
    mFeedbackSlider.setValue(*mFeedbackParameter); //sets the slider value to the current value of the dry wet parameter in the processor
    addAndMakeVisible(mFeedbackSlider); //makes the slider visible on the screen
    
    mFeedbackSlider.onValueChange = [this, mFeedbackParameter] { *mFeedbackParameter = mFeedbackSlider.getValue(); }; //lambda function to update slider when the parameter value is changed
    mFeedbackSlider.onDragStart = [mFeedbackParameter] { mFeedbackParameter->beginChangeGesture(); }; //lambda for what happens when you start dragging the slider
    mFeedbackSlider.onDragEnd = [mFeedbackParameter] { mFeedbackParameter->endChangeGesture(); }; //lambda for what happens when you stop moving the slider
    
    /*This bit is the Feedback Slider*/
    
    //AudioParameterFloat* mDelayTimeParameter = (AudioParameterFloat*)params.getUnchecked(2); //set to 2 because it is the 3rd parameter - indexing from 0
    
    //mDelayTimeSlider.setBounds(200, 0, 100, 100); //sets the position and size
    //mDelayTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag); //sets style
    //mDelayTimeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0); //gets rid of the slider text box from the UI
    //mDelayTimeSlider.setRange(mDelayTimeParameter->range.start, mDelayTimeParameter->range.end); //sets the range
    //mDelayTimeSlider.setValue(*mDelayTimeParameter); //sets the slider value to the current value of the dry wet parameter in the processor
    //addAndMakeVisible(mDelayTimeSlider); //makes the slider visible on the screen
    
    //mDelayTimeSlider.onValueChange = [this, mDelayTimeParameter] { *mDelayTimeParameter = mDelayTimeSlider.getValue(); }; //lambda function to update slider when the parameter value is changed
    //mDelayTimeSlider.onDragStart = [mDelayTimeParameter] { mDelayTimeParameter->beginChangeGesture(); }; //lambda for what happens when you start dragging the slider
    //mDelayTimeSlider.onDragEnd = [mDelayTimeParameter] { mDelayTimeParameter->endChangeGesture(); }; //lambda for what happens when you stop moving the slider
    
}


ChorusFlangerAudioProcessorEditor::~ChorusFlangerAudioProcessorEditor()
{
}

//==============================================================================
void ChorusFlangerAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (22.0f);
    g.drawFittedText ("Wilson Guitars", getLocalBounds(), Justification::centred, 1);
}

void ChorusFlangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
