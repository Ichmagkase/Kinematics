using Godot;
using System.Collections.Generic;

namespace Game
{
	public partial class GlobalConfig : Node
	{
		public const string ConfigPath = "res://config/GlobalConfig.ini";
		public static GlobalConfig Instance { get; private set; }

		public string GameScenePath;
		public string PauseScenePath;
		public string PauseActionName;
		public string HomeScenePath;
		public string FontPath;
		public string GameAudioThemesPath;
		public string GameAudioPath;
		public string PlayerPath;
		public int PlayerCount;
		public List<string> ToolTips;

		// Config section
		private const string SectionDefault = "default";

		// Config keys
		private const string KeyGameScene          = "game_scene";
		private const string KeyPauseScene         = "pause_scene";
		private const string KeyPauseAction        = "pause_action";
		private const string KeyHomeScene          = "home_scene";
		private const string KeyFont               = "font";
		private const string KeyAudioThemes        = "audio_themes";
		private const string KeyCurrentThemePath   = "current_theme_path";
		private const string KeyTooltips           = "tooltips";
		private const string KeyPlayerCount		   = "player_count";
		private const string KeyPlayerPath		   = "player_scene";

		// Default values
		private const string DefaultGameScene        = "res://scenes/game-menu-layer.tscn";
		private const string DefaultPauseScene       = "res://scenes/ui/menus/pause-menu.tscn";
		private const string DefaultPauseAction      = "pause";
		private const string DefaultHomeScene        = "res://scenes/ui/menus/main-menu.tscn";
		private const string DefaultFont             = "res://fonts/Jersey10-Regular.ttf";
		private const string DefaultAudioThemesPath  = "res://music/themes";
		private const string DefaultAudioPath        = "res://music/themes/battle-theme-main.wav";
		private const string DefaultPlayerPath	 	 = "res://scenes/player/player.tscn";
		private const int DefaultPlayerCount 	 	 = 2;

		private ConfigFile _globalConfigFile;

		public override void _Ready()
		{

			if (Instance != null)
			{
				QueueFree();
				return;
			}
			Instance = this;
			
			_globalConfigFile = new ConfigFile();
			Error err = _globalConfigFile.Load(ConfigPath);
			if (err != Error.Ok)
			{
				GD.PrintErr($"Failed to load config file: {ConfigPath}: {err}");
				LoadDefaults();
				return;
			}

			GameScenePath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyGameScene,        DefaultGameScene);
			PauseScenePath      = (string)_globalConfigFile.GetValue(SectionDefault, KeyPauseScene,       DefaultPauseScene);
			PauseActionName     = (string)_globalConfigFile.GetValue(SectionDefault, KeyPauseAction,      DefaultPauseAction);
			HomeScenePath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyHomeScene,        DefaultHomeScene);
			FontPath            = (string)_globalConfigFile.GetValue(SectionDefault, KeyFont,             DefaultFont);
			GameAudioThemesPath = (string)_globalConfigFile.GetValue(SectionDefault, KeyAudioThemes,      DefaultAudioThemesPath);
			GameAudioPath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyCurrentThemePath, DefaultAudioPath);
			PlayerPath          = (string)_globalConfigFile.GetValue(SectionDefault, KeyPlayerPath,       DefaultPlayerPath);
			PlayerCount		    = (int)_globalConfigFile.GetValue(SectionDefault,    KeyPlayerCount, 	  DefaultPlayerCount);

			ToolTips = new List<string>();
			var rawTooltips = _globalConfigFile.GetValue(SectionDefault, KeyTooltips, new Godot.Collections.Array());
			foreach (var tip in (Godot.Collections.Array)rawTooltips)
			{
				ToolTips.Add(tip.ToString());
			}
		}

		private void LoadDefaults()
		{
			GameScenePath       = DefaultGameScene;
			PauseScenePath      = DefaultPauseScene;
			PauseActionName     = DefaultPauseAction;
			HomeScenePath       = DefaultHomeScene;
			FontPath            = DefaultFont;
			GameAudioThemesPath = DefaultAudioThemesPath;
			GameAudioPath       = DefaultAudioPath;
			PlayerCount			= DefaultPlayerCount;
			PlayerPath		    = DefaultPlayerPath;

			ToolTips = new List<string>
			{
				"A Xbox Kinect? What are you gonna do with that?!?",
				"The word 'Kinematics' was introduced in the 19th century by the French physicist Ampère",
                "The software docuemntation for the Kienct library sucks."
			};
		}

		private void SaveConfigFile()
		{
			Error err = _globalConfigFile.Save(ConfigPath);
			if (err != Error.Ok)
			{
				GD.PrintErr($"Failed to save config file: {ConfigPath}: {err}");
			}
		}

		public void SetGameAudioPath(string themePath)
		{
			GameAudioPath = themePath;
			_globalConfigFile.SetValue(SectionDefault, KeyCurrentThemePath, themePath);			
			SaveConfigFile();
		}
	}
}
