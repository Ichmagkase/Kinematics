using Godot;
using System.Collections.Generic;
using System.Linq;

namespace Game
{
	public partial class GlobalConfig : Node
	{
		public const string DefaultConfigPath = "res://config/GlobalConfig.ini";
		public const string PersistantConfigPath = "user://config/GlobalConfig.ini";

		public static GlobalConfig Instance { get; private set; }

		public string GameScenePath;
		public string PauseScenePath;
		public string PauseActionName;
		public string HomeScenePath;
		public string FontPath;
		public string GameAudioThemesPath;
		public string GameAudioPath;
		public string PlayerPath;
		public string PlayerHudPath;
		public string PlayerSoundEffectPath;
		public string MainMenuThemePath;
		public string EndScenePath;
		public string EndSceneThemePath;
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
		private const string KeyPlayerCount		   = "player_count";
		private const string KeyPlayerPath		   = "player_scene";
		private const string KeyPlayerHudPath 	   = "player_hud_scene";
		private const string KeyPlayerSoundEffectPath = "player_sound_effects";
		private const string KeyMainMenuThemePath 	= "main_menu_theme_path";
		private const string KeyEndScenePath 		= "end_scene";
		private const string KeyEndSceneThemePath 	= "end_screen_theme_path";

		// Default values
		private const string DefaultGameScene        = "res://scenes/game-menu-layer.tscn";
		private const string DefaultPauseScene       = "res://scenes/ui/menus/pause-menu.tscn";
		private const string DefaultPauseAction      = "pause";
		private const string DefaultHomeScene        = "res://scenes/ui/menus/main-menu.tscn";
		private const string DefaultFont             = "res://fonts/Jersey10-Regular.ttf";
		private const string DefaultAudioThemesPath  = "res://music/themes";
		private const string DefaultAudioPath        = "res://music/themes/battle-theme-main.wav";
		private const string DefaultPlayerPath	 	 = "res://scenes/player/player.tscn";
		private const string DefaultPlayerHudPath	 = "res://scenes/ui/player/players-hud.tscn";
		private const int DefaultPlayerCount 	 	 = 2;
		private const string DefaultPlayerSoundEffectPath = "res://music/sound-effects/player";
		private const string DefaultMainMenuThemePath = "res://music/themes/main-menu.wav";
		private const string DefaultEndScenePath 	  = "res://scenes/ui/menus/end-screen.tscn";
		private const string DefaultEndSceneThemePath = "res://music/exclusive/you win.wav";

		private ConfigFile _globalConfigFile;
		private bool _canPersistConfig = false;

		private Error CreateDirectoryToPathIfNotExists(string path)
		{
			string dir = path.GetBaseDir();
			if (!DirAccess.DirExistsAbsolute(dir))
			{
				return DirAccess.MakeDirRecursiveAbsolute(dir);
			}
			return Error.Ok;
		}

		public override void _Ready()
		{

			if (Instance != null)
			{
				QueueFree();
				return;
			}
			Instance = this;

			_globalConfigFile = new ConfigFile();

			Error persistConfigErr = _globalConfigFile.Load(PersistantConfigPath);

			// Load defaults if persistant config is not available.
			if (persistConfigErr != Error.Ok)
			{
				_globalConfigFile.Clear();
				Error defaultConfigErr = _globalConfigFile.Load(DefaultConfigPath);
				if (defaultConfigErr != Error.Ok)
				{
					GD.PrintErr($"Failed to load default config file: {DefaultConfigPath}: {defaultConfigErr}");
					GD.PrintErr("Warning: Loading internal defaults and not persisting config");
					LoadDefaults();
					return;
				}
			}

			// If the persistant config does not exist, create it using the loaded default config.
			if (persistConfigErr == Error.FileNotFound)
			{
				Error createDirErr = CreateDirectoryToPathIfNotExists(PersistantConfigPath);
				if (createDirErr != Error.Ok)
				{
					GD.PrintErr($"Failed to create directories to persistant config: {PersistantConfigPath}: {createDirErr}");
					GD.PrintErr($"Warning: Not persisting config");
				}
				else
				{
					Error saveErr = _globalConfigFile.Save(PersistantConfigPath);
					if (saveErr != Error.Ok)
					{
						GD.PrintErr($"Failed to create persistant config file: {PersistantConfigPath}: {saveErr}");
					}
					_canPersistConfig = true;
				}
			}
			else
			{
				_canPersistConfig = true;
			}

			GameScenePath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyGameScene,        DefaultGameScene);
			PauseScenePath      = (string)_globalConfigFile.GetValue(SectionDefault, KeyPauseScene,       DefaultPauseScene);
			PauseActionName     = (string)_globalConfigFile.GetValue(SectionDefault, KeyPauseAction,      DefaultPauseAction);
			HomeScenePath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyHomeScene,        DefaultHomeScene);
			FontPath            = (string)_globalConfigFile.GetValue(SectionDefault, KeyFont,             DefaultFont);
			GameAudioThemesPath = (string)_globalConfigFile.GetValue(SectionDefault, KeyAudioThemes,      DefaultAudioThemesPath);
			GameAudioPath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyCurrentThemePath, DefaultAudioPath);
			PlayerPath          = (string)_globalConfigFile.GetValue(SectionDefault, KeyPlayerPath,       DefaultPlayerPath);
			PlayerHudPath       = (string)_globalConfigFile.GetValue(SectionDefault, KeyPlayerHudPath,       DefaultPlayerHudPath);
			PlayerCount		    = (int)_globalConfigFile.GetValue(SectionDefault,    KeyPlayerCount, 	  DefaultPlayerCount);
			PlayerSoundEffectPath = (string)_globalConfigFile.GetValue(SectionDefault,    KeyPlayerSoundEffectPath, 	  DefaultPlayerSoundEffectPath);
			MainMenuThemePath = (string)_globalConfigFile.GetValue(SectionDefault,    KeyMainMenuThemePath, 	  DefaultMainMenuThemePath);
			EndScenePath = (string)_globalConfigFile.GetValue(SectionDefault,    KeyEndScenePath, 	  DefaultEndScenePath);
			EndSceneThemePath = (string)_globalConfigFile.GetValue(SectionDefault,    KeyEndSceneThemePath, 	  DefaultEndSceneThemePath);

			ToolTips = new List<string>
			{
				"A Xbox Kinect? What are you gonna do with that?!?",
				"The word 'Kinematics' was introduced in the 19th century by the French physicist Ampère",
				"The software docuemntation for the Kienct library sucks.",
				"Eat, sleep, KINECT",
				"Call me on my Kinect.",
				"The world is in your hands. And your Kinect.",
				"Microsoft considered the name 'DirectX Box' for the OG Xbox.",
				"SHING SHING SHING SHING SHING SHING SHING SHING",
				"No skeletons were hurt in the making of this video game.", 
				"I ate that yogurt already??", 
				"Tip: To trigger a jump, you must jump 16 feet in the air", 
				"Just Enable It."
			};

		}

		private void LoadDefaults()
		{
			GameScenePath = DefaultGameScene;
			PauseScenePath = DefaultPauseScene;
			PauseActionName = DefaultPauseAction;
			HomeScenePath = DefaultHomeScene;
			FontPath = DefaultFont;
			GameAudioThemesPath = DefaultAudioThemesPath;
			GameAudioPath = DefaultAudioPath;
			PlayerCount = DefaultPlayerCount;
			PlayerPath = DefaultPlayerPath;

			ToolTips = new List<string>
			{
				"A Xbox Kinect? What are you gonna do with that?!?",
				"The word 'Kinematics' was introduced in the 19th century by the French physicist Ampère",
				"The software docuemntation for the Kienct library sucks."
			};
		}

		private void SaveIfCanPersist()
		{
			if (_canPersistConfig)
			{
				Error err = _globalConfigFile.Save(PersistantConfigPath);
				if (err != Error.Ok)
				{
					GD.PrintErr($"Failed to save to persistant config: {PersistantConfigPath}: {err}");
				}
			}
		}

		public void SetGameAudioPath(string themePath)
		{
			GameAudioPath = themePath;
			_globalConfigFile.SetValue(SectionDefault, KeyCurrentThemePath, themePath);
			SaveIfCanPersist();
		}
	}
}
