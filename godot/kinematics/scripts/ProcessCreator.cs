using Godot;
using System;

namespace Game.Processes
{
	public partial class ProcessCreator : Node
	{
		private const string KINECT_BRIDGE_FILENAME = "KinectVisionLayer.exe";
		
		private const string SERVICE_NAME = "ProcessCreator";

		private string CreatePathInExecDir(string filename)
		{
			return OS.GetExecutablePath().GetBaseDir() + "/" + filename;
		}

		// Called when the node enters the scene tree for the first time.
		public override void _Ready()
		{
			// Only automatically create bridge process if running exported executable.
			if (!OS.HasFeature("editor"))
			{
				string path = CreatePathInExecDir(KINECT_BRIDGE_FILENAME);
				int pid = OS.CreateProcess(path, []);
				if (pid == -1)
				{
					Game.Utils.Logger.PrintErr(
						SERVICE_NAME, $"Could not create {KINECT_BRIDGE_FILENAME} process"
					);
				}
			}
		}

		// Called every frame. 'delta' is the elapsed time since the previous frame.
		public override void _Process(double delta)
		{
		}
	}
}
