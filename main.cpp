#include <iostream>
#include <QCoreApplication>
#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/Bus>
#include <QGst/Pipeline>
#include <QGst/Parse>
#include <QGst/Message>
#include <QGst/Utils/ApplicationSink>
#include <QGst/Utils/ApplicationSource>

using namespace QGst;
using namespace QGlib;

class MySink : public QGst::Utils::ApplicationSink
{
public:
		MySink(QGst::Utils::ApplicationSource *src)
				: QGst::Utils::ApplicationSink(), m_src(src) {}
protected:
		virtual void eos() override
		{
				m_src->endOfStream();
		}
		virtual QGst::FlowReturn newSample() override
		{
			qDebug() << "new frame";
			return QGst::FlowOk;
		}
private:
		QGst::Utils::ApplicationSource *m_src;
};

class FrameReceiver : public QCoreApplication
{
public:
		FrameReceiver(int argc, char **argv);
		~FrameReceiver();
		void start();
private:
		void onBusMessage(const QGst::MessagePtr & message);
private:
		QGst::PipelinePtr m_receivePipeline_ptr;
		QGst::Utils::ApplicationSource m_src;
		MySink m_sink;
};


FrameReceiver::FrameReceiver(int argc, char **argv)
		: QCoreApplication(argc, argv), m_sink(&m_src)
{

		if (argc <= 1) {
				std::cerr << "Usage" << argv[0] << " <port value>" <<  std::endl;
				std::exit(1);
		}
		const char *caps = "application/x-rtp";
		/* source pipeline */
		QString pipe1Descr = QString("udpsrc port=\"%1\" caps=\"%2\" ! "
																		"rtpgstdepay ! "
																		"jpegdec ! "
																		"videoconvert ! "
//																		"autovideosink").arg(argv[1], caps);
																		"appsink name=\"mysink\"").arg(argv[1], caps);
		m_receivePipeline_ptr = QGst::Parse::launch(pipe1Descr).dynamicCast<QGst::Pipeline>();
		m_sink.setElement(m_receivePipeline_ptr->getElementByName("mysink"));
		QGlib::connect(m_receivePipeline_ptr->bus(), "message::error", this, &FrameReceiver::onBusMessage);
		m_receivePipeline_ptr->bus()->addSignalWatch();
}

void FrameReceiver::start()
{
	m_receivePipeline_ptr->setState(QGst::StatePlaying);
}

FrameReceiver::~FrameReceiver()
{
		m_receivePipeline_ptr->setState(QGst::StateNull);
}


void FrameReceiver::onBusMessage(const QGst::MessagePtr & message)
{
		switch (message->type()) {
		case QGst::MessageEos:
			qDebug() << "End of stream";
			quit();
			break;
		case QGst::MessageError:
				qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
				break;
		default:
				break;
		}
}


int main(int argc, char **argv)
{
		QGst::init(&argc, &argv);
		FrameReceiver p(argc, argv);
		p.start();
		return p.exec();
}
