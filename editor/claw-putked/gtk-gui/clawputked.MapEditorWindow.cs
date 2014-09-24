
// This file has been generated by the GUI designer. Do not modify.
namespace clawputked
{
	public partial class MapEditorWindow
	{
		private global::Gtk.HBox hbox1;
		private global::Gtk.ScrolledWindow m_scrolledWindow;
		private global::Gtk.VBox vbox2;
		private global::Gtk.VButtonBox vbuttonbox1;
		private global::Gtk.Button button1;
		private global::Gtk.TreeView m_layers;

		protected virtual void Build ()
		{
			global::Stetic.Gui.Initialize (this);
			// Widget clawputked.MapEditorWindow
			this.Name = "clawputked.MapEditorWindow";
			this.Title = global::Mono.Unix.Catalog.GetString ("MapEditorWindow");
			this.WindowPosition = ((global::Gtk.WindowPosition)(4));
			// Container child clawputked.MapEditorWindow.Gtk.Container+ContainerChild
			this.hbox1 = new global::Gtk.HBox ();
			this.hbox1.Name = "hbox1";
			this.hbox1.Spacing = 6;
			// Container child hbox1.Gtk.Box+BoxChild
			this.m_scrolledWindow = new global::Gtk.ScrolledWindow ();
			this.m_scrolledWindow.CanFocus = true;
			this.m_scrolledWindow.Name = "m_scrolledWindow";
			this.m_scrolledWindow.VscrollbarPolicy = ((global::Gtk.PolicyType)(0));
			this.m_scrolledWindow.HscrollbarPolicy = ((global::Gtk.PolicyType)(0));
			this.m_scrolledWindow.ShadowType = ((global::Gtk.ShadowType)(1));
			this.hbox1.Add (this.m_scrolledWindow);
			global::Gtk.Box.BoxChild w1 = ((global::Gtk.Box.BoxChild)(this.hbox1 [this.m_scrolledWindow]));
			w1.Position = 0;
			// Container child hbox1.Gtk.Box+BoxChild
			this.vbox2 = new global::Gtk.VBox ();
			this.vbox2.Name = "vbox2";
			this.vbox2.Spacing = 6;
			// Container child vbox2.Gtk.Box+BoxChild
			this.vbuttonbox1 = new global::Gtk.VButtonBox ();
			this.vbuttonbox1.Name = "vbuttonbox1";
			this.vbuttonbox1.Spacing = 4;
			this.vbuttonbox1.BorderWidth = ((uint)(6));
			this.vbuttonbox1.LayoutStyle = ((global::Gtk.ButtonBoxStyle)(3));
			// Container child vbuttonbox1.Gtk.ButtonBox+ButtonBoxChild
			this.button1 = new global::Gtk.Button ();
			this.button1.CanFocus = true;
			this.button1.Name = "button1";
			this.button1.UseUnderline = true;
			this.button1.Label = global::Mono.Unix.Catalog.GetString ("Save");
			this.vbuttonbox1.Add (this.button1);
			global::Gtk.ButtonBox.ButtonBoxChild w2 = ((global::Gtk.ButtonBox.ButtonBoxChild)(this.vbuttonbox1 [this.button1]));
			w2.Expand = false;
			w2.Fill = false;
			this.vbox2.Add (this.vbuttonbox1);
			global::Gtk.Box.BoxChild w3 = ((global::Gtk.Box.BoxChild)(this.vbox2 [this.vbuttonbox1]));
			w3.Position = 0;
			// Container child vbox2.Gtk.Box+BoxChild
			this.m_layers = new global::Gtk.TreeView ();
			this.m_layers.CanFocus = true;
			this.m_layers.Name = "m_layers";
			this.vbox2.Add (this.m_layers);
			global::Gtk.Box.BoxChild w4 = ((global::Gtk.Box.BoxChild)(this.vbox2 [this.m_layers]));
			w4.Position = 1;
			this.hbox1.Add (this.vbox2);
			global::Gtk.Box.BoxChild w5 = ((global::Gtk.Box.BoxChild)(this.hbox1 [this.vbox2]));
			w5.Position = 1;
			w5.Expand = false;
			w5.Fill = false;
			this.Add (this.hbox1);
			if ((this.Child != null)) {
				this.Child.ShowAll ();
			}
			this.DefaultWidth = 663;
			this.DefaultHeight = 624;
			this.Show ();
			this.button1.Clicked += new global::System.EventHandler (this.OnSaveClicked);
			this.m_layers.RowActivated += new global::Gtk.RowActivatedHandler (this.OnRowActivated);
			this.m_layers.CursorChanged += new global::System.EventHandler (this.OnCursorchanged);
		}
	}
}