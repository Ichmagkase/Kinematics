using Godot;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System;

namespace Game
{
	public partial class MainMenu : Control
	{
		private List<string> _battleThemesPaths;
		private List<string> _battleThemesNames;
		private AudioStreamPlayer2D _musicPlayer;
		private string _currBattleTheme;
		private VBoxContainer _musicContainer;
		private GlobalConfig _config;

		private void playCurrentTheme()
		{
			if (_currBattleTheme == null) return;
			_musicPlayer.Stream = (AudioStream)GD.Load(_currBattleTheme);
			_musicPlayer.Play();
		}

		private void _OnMusicButtonPressed(string songName)
		{

			if (songName == "None")
			{
				_currBattleTheme = null;
				_config.SetGameAudioPath("");
				_musicPlayer.Stop();
				return;
			}

			int i = 0;
			foreach (string name in _battleThemesNames) 
			{
				if (name == songName)
				{
					_currBattleTheme = _battleThemesPaths[i];
					playCurrentTheme();
					_config.SetGameAudioPath(_currBattleTheme);
					return;
				}
				i++;
			}
			
		}

		private void GetBattleThemes()
		{
			string[] files = DirAccess.GetFilesAt(_config.GameAudioThemesPath);
			string[] fullPaths = new string[files.Length];
			for (int i = 0; i < files.Length; i++)
			{
				fullPaths[i] = _config.GameAudioThemesPath.PathJoin(files[i]);
			}
			
			string pattern = @"^.*\.wav$";
			int k = 0;
			foreach (string path in fullPaths)
			{
				if (Regex.IsMatch(path, pattern) && path != _config.MainMenuThemePath)
				{
					_battleThemesPaths.Add(path);
					_battleThemesNames.Add(files[k].Substring(0, files[k].Length - 4));
				}
				k++;
			}

			_currBattleTheme = _config.GameAudioPath;
			_battleThemesNames.Add("None"); // For Mute
			
		}

		private void LoadBattleNames()
		{
			_musicContainer = GetNode<VBoxContainer>("VBoxContainer/Control/MusicContainer/VBoxContainer");
			if (_musicContainer == null)
			{
				GD.PrintErr("failed to get music container for soundtracks");
			}

			var fontFamily = (Font)GD.Load(_config.FontPath);
			
			foreach (string name in _battleThemesNames)
			{
				Button musicButton = new Button();
				musicButton.Text = name;
				musicButton.AddThemeFontOverride("font", fontFamily);
				musicButton.AddThemeFontSizeOverride("font_size", 30);
				musicButton.Pressed += () => _OnMusicButtonPressed(name);
				_musicContainer.AddChild(musicButton);
			}
		}

		private void LoadToolTipString()
		{
			if (_config.ToolTips != null)
			{
				var tooltips = _config.ToolTips.ToArray();
				Random rnd = new Random();
				int index = rnd.Next(0, tooltips.Length);

				var toolTipLabel = GetNode<Label>("ToolTipLabel");
				toolTipLabel.Text = tooltips[index];
			}

		}

		public override void _Ready()
		{
			_config = GlobalConfig.Instance;
			_musicPlayer = GetNode<AudioStreamPlayer2D>("AudioStreamPlayer2D");
			_battleThemesPaths = new List<string>();
			_battleThemesNames = new List<string>();
			GetBattleThemes();
			LoadBattleNames();
			LoadToolTipString();
			
		}
	}
}
