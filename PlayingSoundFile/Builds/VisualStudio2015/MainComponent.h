#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainContentComponent : public AudioAppComponent, public ChangeListener, public Button::Listener, public AudioTransportSource
{
public:
	MainContentComponent();
	~MainContentComponent();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

	void getNextAudioBlock(const AudioSourceChannelInfo & bufferToFill) override;

	void releaseResources() override;

	void paint(Graphics & g) override;

	void resized() override;

	void changeListenerCallback(ChangeBroadcaster * source) override;

	void buttonClicked(Button * button) override;

private:
	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Stopping
	};

	void changeState(TransportState newState);

	void openButtonClicked();
	void playButtonClicked();
	void stopButtonClicked();

	TransportState state;
	TextButton openButton;
	TextButton playButton;
	TextButton stopButton;
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};

