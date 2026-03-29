using Godot;
using System;
using System.Reflection;
using System.Collections.Generic;

namespace Game.Player
{
	public partial class PlayerConfig : Node
	{
		private string _configPath = "res://config/player/PlayerConfig.ini";
		private ConfigFile _configFile = new ConfigFile();

		// [default]
		private string _defaultSectionName = "default";
		public float Speed = 0.0f;
		public float JumpVelocity = 0.0f;
		public bool CanDoubleJump = false;

		// [boundingbox]
		private string _boundingBoxSectionName = "boundingbox";
		public Dictionary<string, Vector2> AnimationNameToBoundingBoxSize = new Dictionary<string, Vector2>();

		// [boundingboxoffsets]
		private string _boundingBoxOffsetSectionName = "boundingboxoffsets";
		public Dictionary<string, Vector2> AnimationNameToBoundingBoxOffsetSize = new Dictionary<string, Vector2>();

		public override void _Ready()
		{
			LoadSettings();
		}

		public void LoadSettings()
		{
			Error err = _configFile.Load(_configPath);
			if (err != Error.Ok) return;

			string[] keys = _configFile.GetSectionKeys(_defaultSectionName);
			foreach (string key in keys)
			{
				FieldInfo field = GetType().GetField(key, BindingFlags.Public | BindingFlags.Instance);
				if (field != null)
				{
					try
					{
						Variant value = _configFile.GetValue(_defaultSectionName, key);
						object converted = Convert.ChangeType(value.Obj, field.FieldType);
						field.SetValue(this, converted);
					}
					catch (Exception e)
					{
						GD.PrintErr($"Failed to set field {field} from configuration file {_configPath}: {e.Message}");
					}
				}
			}

			keys = _configFile.GetSectionKeys(_boundingBoxSectionName);
			foreach (string key in keys)
			{
				Variant value = _configFile.GetValue(_boundingBoxSectionName, key);
				AnimationNameToBoundingBoxSize[key] = value.AsVector2();
			}

			keys = _configFile.GetSectionKeys(_boundingBoxOffsetSectionName);
			foreach (string key in keys)
			{
				Variant value = _configFile.GetValue(_boundingBoxOffsetSectionName, key);
				AnimationNameToBoundingBoxOffsetSize[key] = value.AsVector2();
			}
		}
	}
}
