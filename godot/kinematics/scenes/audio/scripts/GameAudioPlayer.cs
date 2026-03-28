using Godot;
using System;

namespace Game
{
	public partial class GameAudioPlayer : AudioStreamPlayer2D
	{
		private GlobalConfig _config;
		public override void _Ready()
		{
			_config = GlobalConfig.Instance;
			Stream = (AudioStream)GD.Load(_config.GameAudioPath);
			Play();
		}
	}
}
