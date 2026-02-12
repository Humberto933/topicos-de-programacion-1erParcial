#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

// Use pointers for OpenCV objects
using namespace cv;

namespace OpenCV_TProyectoEvaluacion {

    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void)
        {
            // Initialize components container FIRST!
            components = gcnew System::ComponentModel::Container();

            InitializeComponent();
            InitializeCustomComponents();

            // Initialize OpenCV objects as pointers
            originalImage = new Mat();
            processedImage = new Mat();
            cap = new VideoCapture();
            isCameraActive = false;
            currentFilter = 0;
            trackBarValue = 128;
            trackBar2Value = 50;
            invertThreshold = false;
        }

    protected:
        ~MyForm()
        {
            if (components)
            {
                delete components;
            }

            // Clean up OpenCV objects
            if (cap != nullptr)
            {
                if (cap->isOpened())
                {
                    cap->release();
                }
                delete cap;
                cap = nullptr;
            }

            if (originalImage != nullptr)
            {
                delete originalImage;
                originalImage = nullptr;
            }

            if (processedImage != nullptr)
            {
                delete processedImage;
                processedImage = nullptr;
            }
        }

    private:
        System::Windows::Forms::PictureBox^ pictureBox1;
        System::Windows::Forms::Button^ btnCargar;
        System::Windows::Forms::Button^ btnCamara;
        System::Windows::Forms::ComboBox^ comboFiltros;
        System::Windows::Forms::TrackBar^ trackBar1;
        System::Windows::Forms::Timer^ timer1;

        System::ComponentModel::Container^ components;

        // Custom components
        System::Windows::Forms::Label^ lblTrackBar;
        System::Windows::Forms::Label^ lblFiltroInfo;
        System::Windows::Forms::CheckBox^ chkInvertThreshold;
        System::Windows::Forms::TrackBar^ trackBar2;
        System::Windows::Forms::Label^ lblTrackBar2;

        // OpenCV pointers (unmanaged)
        Mat* originalImage;
        Mat* processedImage;
        VideoCapture* cap;

        bool isCameraActive;
        int currentFilter;
        int trackBarValue;
        int trackBar2Value;
        bool invertThreshold;

        void InitializeCustomComponents()
        {
            // Initialize all custom controls
            this->lblTrackBar = (gcnew System::Windows::Forms::Label());
            this->lblFiltroInfo = (gcnew System::Windows::Forms::Label());
            this->chkInvertThreshold = (gcnew System::Windows::Forms::CheckBox());
            this->trackBar2 = (gcnew System::Windows::Forms::TrackBar());
            this->lblTrackBar2 = (gcnew System::Windows::Forms::Label());

            // lblTrackBar
            this->lblTrackBar->AutoSize = true;
            this->lblTrackBar->Location = System::Drawing::Point(12, 330);
            this->lblTrackBar->Name = L"lblTrackBar";
            this->lblTrackBar->Size = System::Drawing::Size(69, 13);
            this->lblTrackBar->TabIndex = 5;
            this->lblTrackBar->Text = L"Threshold:";

            // lblFiltroInfo
            this->lblFiltroInfo->AutoSize = true;
            this->lblFiltroInfo->Location = System::Drawing::Point(12, 380);
            this->lblFiltroInfo->Name = L"lblFiltroInfo";
            this->lblFiltroInfo->Size = System::Drawing::Size(100, 13);
            this->lblFiltroInfo->TabIndex = 6;
            this->lblFiltroInfo->Text = L"Filter info";

            // chkInvertThreshold
            this->chkInvertThreshold->AutoSize = true;
            this->chkInvertThreshold->Location = System::Drawing::Point(15, 400);
            this->chkInvertThreshold->Name = L"chkInvertThreshold";
            this->chkInvertThreshold->Size = System::Drawing::Size(113, 17);
            this->chkInvertThreshold->TabIndex = 7;
            this->chkInvertThreshold->Text = L"Invert B/W";
            this->chkInvertThreshold->UseVisualStyleBackColor = true;
            this->chkInvertThreshold->CheckedChanged += gcnew System::EventHandler(this, &MyForm::chkInvertThreshold_CheckedChanged);

            // trackBar2
            this->trackBar2->Location = System::Drawing::Point(12, 430);
            this->trackBar2->Name = L"trackBar2";
            this->trackBar2->Size = System::Drawing::Size(300, 45);
            this->trackBar2->TabIndex = 8;
            this->trackBar2->TickFrequency = 10;
            this->trackBar2->Scroll += gcnew System::EventHandler(this, &MyForm::trackBar2_Scroll);

            // lblTrackBar2
            this->lblTrackBar2->AutoSize = true;
            this->lblTrackBar2->Location = System::Drawing::Point(12, 470);
            this->lblTrackBar2->Name = L"lblTrackBar2";
            this->lblTrackBar2->Size = System::Drawing::Size(72, 13);
            this->lblTrackBar2->TabIndex = 9;
            this->lblTrackBar2->Text = L"Parameter 2:";

            // Add controls to form
            this->Controls->Add(this->lblTrackBar);
            this->Controls->Add(this->lblFiltroInfo);
            this->Controls->Add(this->chkInvertThreshold);
            this->Controls->Add(this->trackBar2);
            this->Controls->Add(this->lblTrackBar2);

            // Configure trackbars
            trackBar1->Minimum = 0;
            trackBar1->Maximum = 255;
            trackBar1->Value = 128;
            trackBar1->TickFrequency = 10;

            trackBar2->Minimum = 0;
            trackBar2->Maximum = 255;
            trackBar2->Value = 50;
            trackBar2->TickFrequency = 10;
            trackBar2->Enabled = false;

            // Configure combobox
            array<System::String^>^ filterItems = gcnew array<System::String^>(12) {
                L"Original",
                    L"Grayscale",
                    L"Threshold B/W",
                    L"Canny Edge",
                    L"Sobel Edge",
                    L"RGB - Red Channel",
                    L"RGB - Green Channel",
                    L"RGB - Blue Channel",
                    L"Rotation 90°",
                    L"Rotation 180°",
                    L"Zoom 2x",
                    L"Zoom 0.5x"
            };

            comboFiltros->Items->AddRange(filterItems);
            comboFiltros->SelectedIndex = 0;

            // Configure timer
            timer1->Interval = 30;
            timer1->Tick += gcnew System::EventHandler(this, &MyForm::timer1_Tick);
        }

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            this->btnCargar = (gcnew System::Windows::Forms::Button());
            this->btnCamara = (gcnew System::Windows::Forms::Button());
            this->comboFiltros = (gcnew System::Windows::Forms::ComboBox());
            this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
            this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
            this->SuspendLayout();

            // pictureBox1 - Change from Zoom to AutoSize
            this->pictureBox1->Location = System::Drawing::Point(12, 12);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(640, 280);
            this->pictureBox1->TabIndex = 0;
            this->pictureBox1->TabStop = false;
            this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize; // Changed from Zoom to AutoSize
            this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;

            // btnCargar
            this->btnCargar->Location = System::Drawing::Point(670, 12);
            this->btnCargar->Name = L"btnCargar";
            this->btnCargar->Size = System::Drawing::Size(100, 40);
            this->btnCargar->TabIndex = 1;
            this->btnCargar->Text = L"Load Image";
            this->btnCargar->UseVisualStyleBackColor = true;
            this->btnCargar->Click += gcnew System::EventHandler(this, &MyForm::btnCargar_Click);

            // btnCamara
            this->btnCamara->Location = System::Drawing::Point(670, 70);
            this->btnCamara->Name = L"btnCamara";
            this->btnCamara->Size = System::Drawing::Size(100, 40);
            this->btnCamara->TabIndex = 2;
            this->btnCamara->Text = L"Open Camera";
            this->btnCamara->UseVisualStyleBackColor = true;
            this->btnCamara->Click += gcnew System::EventHandler(this, &MyForm::btnCamara_Click);

            // comboFiltros
            this->comboFiltros->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->comboFiltros->FormattingEnabled = true;
            this->comboFiltros->Location = System::Drawing::Point(670, 130);
            this->comboFiltros->Name = L"comboFiltros";
            this->comboFiltros->Size = System::Drawing::Size(150, 21);
            this->comboFiltros->TabIndex = 3;
            this->comboFiltros->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboFiltros_SelectedIndexChanged);

            // trackBar1
            this->trackBar1->Location = System::Drawing::Point(12, 310);
            this->trackBar1->Name = L"trackBar1";
            this->trackBar1->Size = System::Drawing::Size(300, 45);
            this->trackBar1->TabIndex = 4;
            this->trackBar1->Scroll += gcnew System::EventHandler(this, &MyForm::trackBar1_Scroll);

            // timer1
            this->timer1->Interval = 30;

            // MyForm
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(850, 500);
            this->Controls->Add(this->trackBar1);
            this->Controls->Add(this->comboFiltros);
            this->Controls->Add(this->btnCamara);
            this->Controls->Add(this->btnCargar);
            this->Controls->Add(this->pictureBox1);
            this->Name = L"MyForm";
            this->Text = L"OpenCV Image Processing";
            this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();
        }
#pragma endregion

        // Event Handlers
    private:
        System::Void btnCargar_Click(System::Object^ sender, System::EventArgs^ e)
        {
            OpenFileDialog^ openFileDialog = gcnew OpenFileDialog();
            openFileDialog->Filter = L"Image Files|*.jpg;*.jpeg;*.png;*.bmp;*.tiff";

            if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
            {
                // Stop camera if active
                if (isCameraActive)
                {
                    btnCamara_Click(sender, e);
                }

                try
                {
                    // Convert managed String to std::string
                    System::String^ filePath = openFileDialog->FileName;
                    IntPtr p = Marshal::StringToHGlobalAnsi(filePath);
                    std::string filePathStr = static_cast<const char*>(p.ToPointer());
                    Marshal::FreeHGlobal(p);

                    // Load image
                    *originalImage = imread(filePathStr);

                    if (!originalImage->empty())
                    {
                        ApplyCurrentFilter();
                    }
                    else
                    {
                        MessageBox::Show(L"Could not load image. File may be corrupted or in an unsupported format.",
                            L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    }
                }
                catch (System::Exception^ ex)
                {
                    MessageBox::Show(L"Error loading image: " + ex->Message,
                        L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        }

        System::Void btnCamara_Click(System::Object^ sender, System::EventArgs^ e)
        {
            if (!isCameraActive)
            {
                try
                {
                    if (cap->open(0))
                    {
                        isCameraActive = true;
                        btnCamara->Text = L"Stop Camera";
                        timer1->Start();
                    }
                    else
                    {
                        MessageBox::Show(L"Could not open camera. Please check if camera is connected.",
                            L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    }
                }
                catch (System::Exception^ ex)
                {
                    MessageBox::Show(L"Error opening camera: " + ex->Message,
                        L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
            else
            {
                isCameraActive = false;
                btnCamara->Text = L"Open Camera";
                timer1->Stop();
                cap->release();

                // Clear images
                if (pictureBox1->Image != nullptr)
                {
                    delete pictureBox1->Image;
                    pictureBox1->Image = nullptr;
                }
                *originalImage = Mat();
                *processedImage = Mat();
            }
        }

        System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e)
        {
            if (isCameraActive && cap != nullptr && cap->isOpened())
            {
                try
                {
                    Mat frame;
                    cap->operator>>(frame);
                    if (!frame.empty())
                    {
                        *originalImage = frame.clone();
                        ApplyCurrentFilter();
                    }
                }
                catch (System::Exception^ ex)
                {
                    System::Diagnostics::Debug::WriteLine(L"Camera error: " + ex->Message);
                }
            }
        }

        System::Void comboFiltros_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
        {
            currentFilter = comboFiltros->SelectedIndex;
            UpdateUIControls();

            if (originalImage != nullptr && !originalImage->empty())
            {
                ApplyCurrentFilter();
            }
        }

        System::Void trackBar1_Scroll(System::Object^ sender, System::EventArgs^ e)
        {
            trackBarValue = trackBar1->Value;

            if (originalImage != nullptr && !originalImage->empty())
            {
                ApplyCurrentFilter();
            }
        }

        System::Void trackBar2_Scroll(System::Object^ sender, System::EventArgs^ e)
        {
            trackBar2Value = trackBar2->Value;

            if (originalImage != nullptr && !originalImage->empty())
            {
                ApplyCurrentFilter();
            }
        }

        System::Void chkInvertThreshold_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
        {
            invertThreshold = chkInvertThreshold->Checked;

            if (originalImage != nullptr && !originalImage->empty() &&
                (currentFilter == 2 || currentFilter == 5 || currentFilter == 6 || currentFilter == 7))
            {
                ApplyCurrentFilter();
            }
        }

        System::Void MyForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
        {
            if (cap != nullptr && cap->isOpened())
            {
                cap->release();
            }

            // Clean up image resources
            if (pictureBox1->Image != nullptr)
            {
                delete pictureBox1->Image;
                pictureBox1->Image = nullptr;
            }
        }

        // Filter Application Methods
    private:
        void ApplyCurrentFilter()
        {
            if (originalImage == nullptr || originalImage->empty()) return;

            Mat result;

            try
            {
                switch (currentFilter)
                {
                case 0: // Original
                    result = originalImage->clone();
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Original image";
                    break;

                case 1: // Grayscale
                    cvtColor(*originalImage, result, COLOR_BGR2GRAY);
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Grayscale filter";
                    break;

                case 2: // Threshold B/W
                {
                    Mat gray;
                    cvtColor(*originalImage, gray, COLOR_BGR2GRAY);
                    int thresholdType = invertThreshold ? THRESH_BINARY_INV : THRESH_BINARY;
                    threshold(gray, result, trackBarValue, 255, thresholdType);
                    trackBar1->Enabled = true;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = true;
                    lblFiltroInfo->Text = L"Threshold: " + trackBarValue.ToString();
                    break;
                }

                case 3: // Canny Edge
                {
                    Mat gray, edges;
                    cvtColor(*originalImage, gray, COLOR_BGR2GRAY);
                    Canny(gray, edges, trackBarValue, trackBarValue * 2);
                    result = edges.clone();
                    trackBar1->Enabled = true;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Canny threshold: " + trackBarValue.ToString();
                    break;
                }

                case 4: // Sobel Edge
                {
                    Mat gray, grad_x, grad_y, abs_grad_x, abs_grad_y;
                    cvtColor(*originalImage, gray, COLOR_BGR2GRAY);

                    Sobel(gray, grad_x, CV_16S, 1, 0, 3);
                    Sobel(gray, grad_y, CV_16S, 0, 1, 3);

                    convertScaleAbs(grad_x, abs_grad_x);
                    convertScaleAbs(grad_y, abs_grad_y);

                    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, result);
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Sobel edge detection";
                    break;
                }

                case 5: // RGB - Red Channel
                {
                    std::vector<Mat> channels(3);
                    split(*originalImage, channels);

                    Mat zeros = Mat::zeros(originalImage->size(), CV_8UC1);
                    channels[0] = zeros; // Blue
                    channels[1] = zeros; // Green
                    // channels[2] is Red - keep as is

                    if (invertThreshold)
                    {
                        threshold(channels[2], channels[2], trackBarValue, 255, THRESH_BINARY_INV);
                    }

                    merge(channels, result);
                    trackBar1->Enabled = true;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = true;
                    lblFiltroInfo->Text = L"Red channel - Threshold: " + trackBarValue.ToString();
                    break;
                }

                case 6: // RGB - Green Channel
                {
                    std::vector<Mat> channels(3);
                    split(*originalImage, channels);

                    Mat zeros = Mat::zeros(originalImage->size(), CV_8UC1);
                    channels[0] = zeros; // Blue
                    // channels[1] is Green - keep as is
                    channels[2] = zeros; // Red

                    if (invertThreshold)
                    {
                        threshold(channels[1], channels[1], trackBarValue, 255, THRESH_BINARY_INV);
                    }

                    merge(channels, result);
                    trackBar1->Enabled = true;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = true;
                    lblFiltroInfo->Text = L"Green channel - Threshold: " + trackBarValue.ToString();
                    break;
                }

                case 7: // RGB - Blue Channel
                {
                    std::vector<Mat> channels(3);
                    split(*originalImage, channels);

                    Mat zeros = Mat::zeros(originalImage->size(), CV_8UC1);
                    // channels[0] is Blue - keep as is
                    channels[1] = zeros; // Green
                    channels[2] = zeros; // Red

                    if (invertThreshold)
                    {
                        threshold(channels[0], channels[0], trackBarValue, 255, THRESH_BINARY_INV);
                    }

                    merge(channels, result);
                    trackBar1->Enabled = true;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = true;
                    lblFiltroInfo->Text = L"Blue channel - Threshold: " + trackBarValue.ToString();
                    break;
                }

                case 8: // Rotation 90°
                {
                    transpose(*originalImage, result);
                    flip(result, result, 1);
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Rotated 90° clockwise";
                    break;
                }

                case 9: // Rotation 180°
                {
                    flip(*originalImage, result, -1);
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Rotated 180°";
                    break;
                }

                case 10: // Zoom 2x
                {
                    cv::Size newSize(originalImage->cols * 2, originalImage->rows * 2);
                    resize(*originalImage, result, newSize, 0, 0, INTER_LINEAR);
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Zoom 2x - Image size: " + newSize.width.ToString() + L"x" + newSize.height.ToString();
                    break;
                }

                case 11: // Zoom 0.5x
                {
                    cv::Size newSize(originalImage->cols / 2, originalImage->rows / 2);
                    resize(*originalImage, result, newSize, 0, 0, INTER_AREA);
                    trackBar1->Enabled = false;
                    trackBar2->Enabled = false;
                    chkInvertThreshold->Enabled = false;
                    lblFiltroInfo->Text = L"Zoom 0.5x - Image size: " + newSize.width.ToString() + L"x" + newSize.height.ToString();
                    break;
                }
                }

                if (!result.empty())
                {
                    *processedImage = result.clone();
                    DisplayImage(result);
                }
            }
            catch (const cv::Exception& e)
            {
                System::String^ errorMsg = gcnew System::String(e.what());
                System::Diagnostics::Debug::WriteLine(L"OpenCV error: " + errorMsg);
            }
            catch (System::Exception^ ex)
            {
                System::Diagnostics::Debug::WriteLine(L"Filter error: " + ex->Message);
            }
            catch (...)
            {
                System::Diagnostics::Debug::WriteLine(L"Unknown unmanaged error occurred");
            }
        }

        void UpdateUIControls()
        {
            // Enable/disable controls based on selected filter
            bool thresholdFilters = (currentFilter == 2 || currentFilter == 5 ||
                currentFilter == 6 || currentFilter == 7);
            bool cannyFilter = (currentFilter == 3);

            trackBar1->Enabled = (thresholdFilters || cannyFilter);
            chkInvertThreshold->Enabled = thresholdFilters;

            if (thresholdFilters)
            {
                lblTrackBar->Text = L"Threshold:";
                trackBar1->Minimum = 0;
                trackBar1->Maximum = 255;
                trackBar1->Value = trackBarValue;
            }
            else if (cannyFilter)
            {
                lblTrackBar->Text = L"Canny Threshold:";
                trackBar1->Minimum = 0;
                trackBar1->Maximum = 255;
                trackBar1->Value = trackBarValue;
            }
        }

        void DisplayImage(Mat& image)
        {
            if (image.empty()) return;

            try
            {
                if (image.channels() == 3)
                {
                    // Convert BGR to RGB for correct color display
                    Mat rgbImage;
                    cvtColor(image, rgbImage, COLOR_BGR2RGB);

                    int width = rgbImage.cols;
                    int height = rgbImage.rows;

                    // Create bitmap
                    Bitmap^ bitmap = gcnew Bitmap(width, height, Imaging::PixelFormat::Format24bppRgb);

                    // Lock bitmap data
                    System::Drawing::Rectangle rect(0, 0, width, height);
                    System::Drawing::Imaging::BitmapData^ bmpData =
                        bitmap->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly,
                            Imaging::PixelFormat::Format24bppRgb);

                    // Copy pixel data row by row
                    int stride = bmpData->Stride;
                    for (int y = 0; y < height; y++)
                    {
                        Byte* destRow = (Byte*)bmpData->Scan0.ToPointer() + y * stride;
                        Byte* srcRow = rgbImage.ptr<Byte>(y);
                        memcpy(destRow, srcRow, width * 3);
                    }

                    bitmap->UnlockBits(bmpData);

                    // Clean up old image and set new one
                    if (pictureBox1->Image != nullptr)
                    {
                        delete pictureBox1->Image;
                        pictureBox1->Image = nullptr;
                    }

                    pictureBox1->Image = bitmap;
                    pictureBox1->Size = System::Drawing::Size(width, height); // Adjust PictureBox size to match image
                }
                else if (image.channels() == 1)
                {
                    int width = image.cols;
                    int height = image.rows;

                    // Create 8bpp indexed bitmap
                    Bitmap^ bitmap = gcnew Bitmap(width, height, Imaging::PixelFormat::Format8bppIndexed);

                    // Set grayscale palette
                    Imaging::ColorPalette^ palette = bitmap->Palette;
                    for (int i = 0; i < 256; i++)
                    {
                        palette->Entries[i] = Color::FromArgb(i, i, i);
                    }
                    bitmap->Palette = palette;

                    // Lock bitmap data
                    System::Drawing::Rectangle rect(0, 0, width, height);
                    System::Drawing::Imaging::BitmapData^ bmpData =
                        bitmap->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly,
                            Imaging::PixelFormat::Format8bppIndexed);

                    // Copy pixel data row by row
                    int stride = bmpData->Stride;
                    for (int y = 0; y < height; y++)
                    {
                        Byte* destRow = (Byte*)bmpData->Scan0.ToPointer() + y * stride;
                        Byte* srcRow = image.ptr<Byte>(y);
                        memcpy(destRow, srcRow, width);
                    }

                    bitmap->UnlockBits(bmpData);

                    // Clean up old image and set new one
                    if (pictureBox1->Image != nullptr)
                    {
                        delete pictureBox1->Image;
                        pictureBox1->Image = nullptr;
                    }

                    pictureBox1->Image = bitmap;
                    pictureBox1->Size = System::Drawing::Size(width, height); // Adjust PictureBox size to match image
                }
            }
            catch (System::Exception^ ex)
            {
                System::Diagnostics::Debug::WriteLine(L"Display error: " + ex->Message);
            }
        }
    };
}