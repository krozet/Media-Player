#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

MainContentComponent::MainContentComponent()
	: state(Stopped)
{

	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open...");
	openButton.addListener(this);

	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.addListener(this);
	playButton.setColour(TextButton::buttonColourId, Colours::lightsteelblue);
	playButton.setEnabled(false);

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.addListener(this);
	stopButton.setColour(TextButton::buttonColourId, Colours::red);
	stopButton.setEnabled(false);

	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);

	setSize(800, 600);

	// specify the number of input and output channels that we want to open
	setAudioChannels(0, 2);
}

MainContentComponent::~MainContentComponent()
{
	shutdownAudio();
}

//==============================================================================
void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainContentComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	if (readerSource == nullptr)
	{
		bufferToFill.clearActiveBufferRegion();
		return;
	}

	transportSource.getNextAudioBlock(bufferToFill);
}

void MainContentComponent::releaseResources()
{
	// This will be called when the audio device stops, or when it is being
	// restarted due to a setting change.
	transportSource.releaseResources();
}

//==============================================================================
void MainContentComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	// You can add your drawing code here!
}

void MainContentComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	openButton.setBounds(10, 10, getWidth() - 20, 20);
	playButton.setBounds(10, 40, getWidth() - 20, 20);
	stopButton.setBounds(10, 70, getWidth() - 20, 20);
}

void MainContentComponent::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case Stopped:                           
			stopButton.setEnabled(false);
			playButton.setEnabled(true);
			transportSource.setPosition(0.0);
			break;

		case Starting:                          
			playButton.setEnabled(false);
			transportSource.start();
			break;

		case Playing:                           
			stopButton.setEnabled(true);
			break;

		case Stopping:                          
			transportSource.stop();
			break;
		}
	}
}

void MainContentComponent::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &transportSource)
	{
		if (transportSource.isPlaying())
			changeState(Playing);
		else
			changeState(Stopped);
	}
}

void MainContentComponent::buttonClicked(Button* button)
{
	if (button == &openButton)  openButtonClicked();
	if (button == &playButton)  playButtonClicked();
	if (button == &stopButton)  stopButtonClicked();
}

void MainContentComponent::openButtonClicked()
{
	FileChooser chooser("Select a Wave file to play...", File::nonexistent, "*.wav");                                        

	if (chooser.browseForFileToOpen())                                   
	{
		File file(chooser.getResult());                                  
		AudioFormatReader* reader = formatManager.createReaderFor(file); 

		if (reader != nullptr)
		{
			ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true); 
			transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                         
			playButton.setEnabled(true);                                                                  
			readerSource = newSource.release();                                                            
		}
	}
}

void MainContentComponent::playButtonClicked()
{
	changeState(Starting);
}

void MainContentComponent::stopButtonClicked()
{
	changeState(Stopping);
}

// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent() { return new MainContentComponent(); }

#endif 