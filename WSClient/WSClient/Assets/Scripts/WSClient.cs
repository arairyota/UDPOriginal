using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using WebSocketSharp;
using WebSocketSharp.Net;
using System.Net;
using System.Net.Sockets;

public class WSClient: MonoBehaviour {

	private const int PORT_NO = 8801;
	private const int LOG_MAX = 20;

	private WebSocket ws;
	private string user;
	private string msg = "";
	private string addr = "";
	private bool connecting;

	private List<string> messages = new List<string>();

	void Start()
	{
		user = "774";
	}
	
	void Update()
	{
	}

	void OnGUI()
	{
		int fsize = Screen.width > Screen.height ? Screen.width / 32 : Screen.height / 32;

		GUI.skin.textField.fontSize	=
		GUI.skin.button.fontSize	=
		GUI.skin.label.fontSize		= fsize;

		GUILayout.BeginVertical (GUILayout.Width(Screen.width / 2));

		if (!connecting) {
			GUILayout.Label("サーバーアドレス");
			addr = GUILayout.TextField (addr);

			if (GUILayout.Button ("Connect") && addr != "" && user != "") {
				ws = new WebSocket ("ws://" + addr + ":" + PORT_NO.ToString () + "/");

				ws.OnOpen += (sender, e) =>
				{
					Debug.Log ("WebSocket Open");
					connecting = true;
				};
				
				ws.OnMessage += (sender, e) =>
				{
					Debug.Log ("WebSocket Message Type: " + e.GetType () + ", Data: " + e.Data);
					messages.Add(e.Data);
					while(messages.Count > LOG_MAX)
					{
						messages.RemoveAt(0);
					}
				};
				
				ws.OnError += (sender, e) =>
				{
					Debug.Log ("WebSocket Error Message: " + e.Message);
				};
				
				ws.OnClose += (sender, e) =>
				{
					Debug.Log ("WebSocket Close");
					connecting = false;
				};
				
				ws.Connect ();
			}
		} else {
			GUILayout.Label("ユーザー名");
			user = GUILayout.TextField (user);

			GUILayout.Label("メッセージ");
			msg = GUILayout.TextField (msg);

			if(GUILayout.Button("Send") && msg != "")
			{
				msg = user + ": " + msg;
				ws.Send(msg);
				msg = "";
			}

			// 入力されたメッセージを表示
			for (int i = messages.Count - 1; i >= 0; i--)
			{
				GUILayout.Label(messages[i]);
			}
		}

		GUILayout.EndVertical ();
	}

	void OnDestroy()
	{
		if (ws != null) {
			ws.Close ();
		}
	}
}
