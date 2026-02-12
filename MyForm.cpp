#include "MyForm.h"

using namespace OpenCV_TProyectoEvaluacion;
using namespace msclr::interop;

//
// ===============================
// Convertir cv::Mat a Bitmap
// ===============================
//
System::Drawing::Bitmap^ MyForm::MatToBitmap(cv::Mat img)
{
    if (img.empty()) return nullptr;

    cv::Mat temp;

    if (img.channels() == 1)
    {
        cv::cvtColor(img, temp, cv::COLOR_GRAY2BGR);
    }
    else
    {
        cv::cvtColor(img, temp, cv::COLOR_BGR2RGB);
    }

    return gcnew System::Drawing::Bitmap(
        temp.cols,
        temp.rows,
        temp.step,
        System::Drawing::Imaging::PixelFormat::Format24bppRgb,
        System::IntPtr(temp.data)
    );
}

//
// ===============================
// Botón Cargar Imagen
// ===============================
//
System::Void MyForm::btnCargar_Click(System::Object^ sender, System::EventArgs^ e)
{
    System::Windows::Forms::OpenFileDialog^ ofd = gcnew System::Windows::Forms::OpenFileDialog();
    ofd->Filter = "Imagenes|*.jpg;*.png;*.bmp";

    if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        std::string ruta = marshal_as<std::string>(ofd->FileName);
        *imagenOriginal = cv::imread(ruta);

        if (!imagenOriginal->empty())
            AplicarFiltro(nullptr, nullptr);
    }
}

//
// ===============================
// Botón Abrir Cámara
// ===============================
//
System::Void MyForm::btnCamara_Click(System::Object^ sender, System::EventArgs^ e)
{
    cap->open(0);

    if (cap->isOpened())
        timer1->Start();
}

//
// ===============================
// Timer (captura en vivo)
// ===============================
//
System::Void MyForm::timer1_Tick(System::Object^ sender, System::EventArgs^ e)
{
    if (!cap->isOpened()) return;

    (*cap) >> (*imagenOriginal);

    if (!imagenOriginal->empty())
        AplicarFiltro(nullptr, nullptr);
}

//
// ===============================
// Aplicar Filtros
// ===============================
//
System::Void MyForm::AplicarFiltro(System::Object^ sender, System::EventArgs^ e)
{
    if (imagenOriginal->empty()) return;

    *imagenProcesada = imagenOriginal->clone();

    int valor = trackBar1->Value;
    System::String^ filtro = comboFiltros->SelectedItem->ToString();

    // =============================

    if (filtro == "Escala de grises")
    {
        cv::cvtColor(*imagenProcesada, *imagenProcesada, cv::COLOR_BGR2GRAY);
    }

    else if (filtro == "Umbral Blanco/Negro")
    {
        cv::cvtColor(*imagenProcesada, *imagenProcesada, cv::COLOR_BGR2GRAY);
        cv::threshold(*imagenProcesada, *imagenProcesada, valor, 255, cv::THRESH_BINARY);
    }

    else if (filtro == "Umbral Negro/Blanco")
    {
        cv::cvtColor(*imagenProcesada, *imagenProcesada, cv::COLOR_BGR2GRAY);
        cv::threshold(*imagenProcesada, *imagenProcesada, valor, 255, cv::THRESH_BINARY_INV);
    }

    else if (filtro == "Canny")
    {
        cv::Mat gray;
        cv::cvtColor(*imagenProcesada, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, *imagenProcesada, valor, valor * 2);
    }

    else if (filtro == "Sobel")
    {
        cv::Mat gray, grad;
        cv::cvtColor(*imagenProcesada, gray, cv::COLOR_BGR2GRAY);
        cv::Sobel(gray, grad, CV_8U, 1, 1);
        *imagenProcesada = grad;
    }

    else if (filtro == "Separar RGB")
    {
        std::vector<cv::Mat> canales;
        cv::split(*imagenProcesada, canales);

        // Mostrar canal Rojo
        *imagenProcesada = canales[2];
    }

    else if (filtro == "Rotación")
    {
        cv::Point2f centro(
            imagenProcesada->cols / 2.0f,
            imagenProcesada->rows / 2.0f
        );

        cv::Mat rot = cv::getRotationMatrix2D(centro, valor, 1.0);
        cv::warpAffine(*imagenProcesada, *imagenProcesada, rot, imagenProcesada->size());
    }

    else if (filtro == "Zoom")
    {
        double escala = 1.0 + (valor / 100.0);
        cv::resize(*imagenProcesada, *imagenProcesada, cv::Size(), escala, escala);
    }

    // =============================

    pictureBox1->Image = MatToBitmap(*imagenProcesada);
}
