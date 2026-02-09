#pragma once

namespace OP26Lab5Graphics {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GForm
	/// </summary>
	public ref class GForm : public System::Windows::Forms::Form
	{
	public:
		GForm(void)
		{
			InitializeComponent(); 
			this->Size = System::Drawing::Size(1224, 500); // Ширина 1024, Висота 768
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>

		void pictureBox1_Paint(Object^ sender, PaintEventArgs^ e)
		{
			Graphics^ g = e->Graphics;
			g->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::AntiAlias;

			// Вихідні дані (Точки та вектори) 428, 266
			PointF p1(100.0f, 200.0f);   // Початкова точка
			PointF p2(300.0f, 50.0f);   // Кінцева точка
			PointF m1(50.0f, -500.0f);   // Вектор на початку
			PointF m2(50.0f, -500.0f);   // Вектор в кінці

			// Малювання векторів (пунктиром)
			Pen^ vectorPen = gcnew Pen(Color::Gray, 1.0f);
			vectorPen->DashStyle = System::Drawing::Drawing2D::DashStyle::Dash;
			g->DrawLine(vectorPen, p1, PointF(p1.X + m1.X, p1.Y + m1.Y));
			g->DrawLine(vectorPen, p2, PointF(p2.X + m2.X, p2.Y + m2.Y));

			// Малювання кривої
			DrawHermite(g, p1, p2, m1, m2);
		}

		System::Void pictureBox2_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
			Graphics^ g = e->Graphics;
			g->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::AntiAlias;

			PointF startPoint(pictureBox2->Width / 2, pictureBox2->Height + 100);
			PointF startVector(0, -100); // Вектор росту вгору

			// Запуск фракталу: початок, вектор, довжина, кут (вгору = -90 град), порядок K
			DrawFern(g, startPoint, startVector, 150.0f, -90.0f, maxK);
		}

		void DrawHermite(Graphics^ g, PointF p1, PointF p2, PointF m1, PointF m2) 
		{
			int segments = 100;
			cli::array<PointF>^ points = gcnew cli::array<PointF>(segments + 1);

			for (int i = 0; i <= segments; i++) {
				float t = (float)i / (float)segments;
				float t2 = t * t;
				float t3 = t2 * t;

				// Поліноми Ерміта
				float h00 = 2 * t3 - 3 * t2 + 1;
				float h01 = t3 - 2 * t2 + t;
				float h10 = -2 * t3 + 3 * t2;
				float h11 = t3 - t2;

				// Розрахунок одної точки для поточного t
				float x = h00 * p1.X + h01 * m1.X + h10 * p2.X + h11 * m2.X;
				float y = h00 * p1.Y + h01 * m1.Y + h10 * p2.Y + h11 * m2.Y;

				points[i] = PointF(x, y);
			}

			// Малюємо суцільну лінію через всі точки
			Pen^ curvePen = gcnew Pen(Color::Blue, 3);
			g->DrawLines(curvePen, points);
		}

		void DrawSingleHermite(Graphics^ g, PointF p0, PointF p1, PointF m0, PointF m1, int k) {
			const int segments = 20;
			cli::array<PointF>^ pts = gcnew cli::array<PointF>(segments + 1);

			for (int i = 0; i <= segments; i++) {
				float t = (float)i / segments;
				float t2 = t * t;
				float t3 = t2 * t;

				float h00 = 2 * t3 - 3 * t2 + 1;
				float h01 = -2 * t3 + 3 * t2;
				float h10 = t3 - 2 * t2 + t;
				float h11 = t3 - t2;

				pts[i] = PointF(
					h00 * p0.X + h10 * m0.X + h01 * p1.X + h11 * m1.X,
					h00 * p0.Y + h10 * m0.Y + h01 * p1.Y + h11 * m1.Y
				);
			}

			// Різний колір для кожного листка
			Color col = Color::FromArgb(30 * k, 200 - (20 * k), 50);
			Pen^ p = gcnew Pen(col, (float)k);
			g->DrawLines(p, pts);
		}

		void DrawFern(Graphics^ g, PointF p0, PointF m0, float length, float angle, int k) {
			if (k <= 0) return;

			// Обчислюємо кінцеву точку p1 на основі кута та довжини
			float rad = angle * Math::PI / 180.0f;
			PointF p1 = PointF(p0.X + length * Math::Cos(rad), p0.Y + length * Math::Sin(rad));

			// Кінцевий вектор m1 (зберігаємо напрямок для плавності)
			PointF m1 = PointF(m0.X * 0.5f, m0.Y * 0.5f);

			// Малюємо поточну гілку Ерміта
			DrawSingleHermite(g, p0, p1, m0, m1, k);

			// Створюємо "пучок" з A нових гілок у точці p1
			float spreadAngle = 60.0f; // загальний розмах пучка
			float step = spreadAngle / (A - 1);

			for (int i = 0; i < A; i++) {
				float newAngle = angle - (spreadAngle / 2) + (i * step);
				// Рекурсивний виклик для наступного порядку K
				DrawFern(g, p1, PointF(p1.X - p0.X, p1.Y - p0.Y), length * 0.7f, newAngle, k - 1);
			}
		}

		~GForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::PictureBox^ pictureBox2;
	//private: System::Windows::Forms::Button^ button1;
	protected:

	private:
		int A = 3; // Кількість відрізків у пучку
		int maxK = 7; // Порядок фракталу
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->Text = "Крива Ерміта";
			this->DoubleBuffered = true;

			this->label1 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			//this->button1 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->label1->Location = System::Drawing::Point(139, 7);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(136, 24);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Lab5_Graphics";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(52, 65);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(428, 266);
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GForm::pictureBox1_Paint);
			this->pictureBox1->BorderStyle = BorderStyle::FixedSingle;
			//
			// pictureBox2
			// 
			this->pictureBox2->Location = System::Drawing::Point(532, 65);
			this->pictureBox2->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->pictureBox2->Name = L"pictureBox1";
			this->pictureBox2->Size = System::Drawing::Size(628, 366);
			this->pictureBox2->TabIndex = 1;
			this->pictureBox2->TabStop = false;
			this->pictureBox2->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &GForm::pictureBox2_Paint);
			this->pictureBox2->BorderStyle = BorderStyle::FixedSingle;

			// 
			// button1
			// 
			/*this->button1->Location = System::Drawing::Point(326, 20);
			this->button1->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(88, 40);
			this->button1->TabIndex = 2;
			this->button1->Text = L"Draw ...";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &GForm::button1_Click);*/
			// 
			// GForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(531, 357);
			//this->Controls->Add(this->button1);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->label1);
			this->Margin = System::Windows::Forms::Padding(2, 2, 2, 2);
			this->Name = L"GForm";
			this->Text = L"GForm";
			this->Load += gcnew System::EventHandler(this, &GForm::GForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void GForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	};
}
