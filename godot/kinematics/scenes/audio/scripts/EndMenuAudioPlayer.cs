using Godot;
using System;

namespace Game
{
	public partial class EndMenuAudioPlayer : AudioStreamPlayer2D
	{
		private GlobalConfig _config;

		public override void _Ready()
		{
			_config = GlobalConfig.Instance;
			if (_config.GameAudioPath == "") return;
			Stream = (AudioStream)GD.Load(_config.EndSceneThemePath);
			Play();
		}

	}
}
