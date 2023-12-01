/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPlayerPluginAudioProcessorEditor::AudioPlayerPluginAudioProcessorEditor (AudioPlayerPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setSize(300, 200);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };
    addAndMakeVisible(&openButton);

    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);
    addAndMakeVisible(&playButton);

    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);
    addAndMakeVisible(&stopButton);

    formatManager.registerBasicFormats();
    audioProcessor.transportSource.addChangeListener(this);
}

AudioPlayerPluginAudioProcessorEditor::~AudioPlayerPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPlayerPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPlayerPluginAudioProcessorEditor::resized()
{
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    playButton.setBounds(10, 40, getWidth() - 20, 20);
    stopButton.setBounds(10, 70, getWidth() - 20, 20);
}

void AudioPlayerPluginAudioProcessorEditor::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
        juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)     // [8]
        {
            auto file = fc.getResult();

            if (file != juce::File{})                                                // [9]
            {
                auto* reader = formatManager.createReaderFor(file);                 // [10]

                if (reader != nullptr)
                {
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);   // [11]
                    audioProcessor.transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);       // [12]
                    playButton.setEnabled(true);                                                      // [13]
                    audioProcessor.readerSource.reset(newSource.release());                                          // [14]
                }
            }
        });
}

void AudioPlayerPluginAudioProcessorEditor::playButtonClicked()
{
    changeState(Starting);
}

void AudioPlayerPluginAudioProcessorEditor::stopButtonClicked()
{
    changeState(Stopping);
}

void AudioPlayerPluginAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &audioProcessor.transportSource)
    {
        if (audioProcessor.transportSource.isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);
    }
}

void AudioPlayerPluginAudioProcessorEditor::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Stopped:                           // [3]
            stopButton.setEnabled(false);
            playButton.setEnabled(true);
            audioProcessor.transportSource.setPosition(0.0);
            break;

        case Starting:                          // [4]
            playButton.setEnabled(false);
            audioProcessor.transportSource.start();
            break;

        case Playing:                           // [5]
            stopButton.setEnabled(true);
            break;

        case Stopping:                          // [6]
            audioProcessor.transportSource.stop();
            break;
        }
    }
}
