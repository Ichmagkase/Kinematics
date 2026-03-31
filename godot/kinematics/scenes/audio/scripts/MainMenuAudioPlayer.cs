using Godot;
using System;

namespace Game
{
	public partial class MainMenuAudioPlayer : AudioStreamPlayer2D
	{
		private GlobalConfig _config;

		private void _OnFinished()
		{
			Play();
		}

		public override void _Ready()
		{
			_config = GlobalConfig.Instance;
			if (_config.GameAudioPath == "") return;
			Stream = (AudioStream)GD.Load(_config.MainMenuThemePath);
			Finished += _OnFinished;
			Play();
		}

	}
}
