/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AudioPlayerPluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::ChangeListener
{
public:
    AudioPlayerPluginAudioProcessorEditor (AudioPlayerPluginAudioProcessor&);
    ~AudioPlayerPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPlayerPluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayerPluginAudioProcessorEditor)

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    //std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    //juce::AudioTransportSource transportSource;

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();

    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    TransportState state;

    void changeState(TransportState newState);
};
