#ifndef M2LCONFIG_H
#define M2LCONFIG_H

#include <QObject>
#include <QtQuick/QQuickItem>

class M2lConfig : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY (int quietMode MEMBER m_quietMode NOTIFY quietModeChanged)
    Q_PROPERTY (QString deposits MEMBER m_deposits NOTIFY depositsChanged)
    Q_PROPERTY (QString dtb MEMBER m_dtb NOTIFY dtbChanged)
    Q_PROPERTY (int orientationDecimate MEMBER m_orientationDecimate NOTIFY orientationDecimateChanged)
    Q_PROPERTY (int contactDecimate MEMBER m_contactDecimate NOTIFY contactDecimateChanged)
    Q_PROPERTY (int intrusionMode MEMBER m_intrusionMode NOTIFY intrusionModeChanged)
    Q_PROPERTY (int interpolationSpacing MEMBER m_interpolationSpacing NOTIFY interpolationSpacingChanged)
    Q_PROPERTY (int misorientation MEMBER m_misorientation NOTIFY misorientationChanged)
    Q_PROPERTY (QString interpolationScheme MEMBER m_interpolationScheme NOTIFY interpolationSchemeChanged)
    Q_PROPERTY (int faultDecimate MEMBER m_faultDecimate NOTIFY faultDecimateChanged)
    Q_PROPERTY (double minFaultLength MEMBER m_minFaultLength NOTIFY minFaultLengthChanged)
    Q_PROPERTY (double faultDip MEMBER m_faultDip NOTIFY faultDipChanged)
    Q_PROPERTY (double plutonDip MEMBER m_plutonDip NOTIFY plutonDipChanged)
    Q_PROPERTY (QString plutonForm MEMBER m_plutonForm NOTIFY plutonFormChanged)
    Q_PROPERTY (double distBuffer MEMBER m_distBuffer NOTIFY distBufferChanged)
    Q_PROPERTY (double contactDip MEMBER m_contactDip NOTIFY contactDipChanged)
    Q_PROPERTY (int contactOrientationDecimate MEMBER m_contactOrientationDecimate NOTIFY contactOrientationDecimateChanged)
    Q_PROPERTY (QString nullScheme MEMBER m_nullScheme NOTIFY nullSchemeChanged)
    Q_PROPERTY (double thicknessBuffer MEMBER m_thicknessBuffer NOTIFY thicknessBufferChanged)
    Q_PROPERTY (double maxThicknessAllowed MEMBER m_maxThicknessAllowed NOTIFY maxThicknessAllowedChanged)
    Q_PROPERTY (int foldDecimate MEMBER m_foldDecimate NOTIFY foldDecimateChanged)
    Q_PROPERTY (double fatStep MEMBER m_fatStep NOTIFY fatStepChanged)
    Q_PROPERTY (double closeDip MEMBER m_closeDip NOTIFY closeDipChanged)
    Q_PROPERTY (bool useInterpolations MEMBER m_useInterpolations NOTIFY useInterpolationsChanged)
    Q_PROPERTY (bool useFat MEMBER m_useFat NOTIFY useFatChanged)

    Q_SIGNALS:
        void quietModeChanged();
        void depositsChanged();
        void dtbChanged();
        void orientationDecimateChanged();
        void contactDecimateChanged();
        void intrusionModeChanged();
        void interpolationSpacingChanged();
        void misorientationChanged();
        void interpolationSchemeChanged();
        void faultDecimateChanged();
        void minFaultLengthChanged();
        void faultDipChanged();
        void plutonDipChanged();
        void plutonFormChanged();
        void distBufferChanged();
        void contactDipChanged();
        void contactOrientationDecimateChanged();
        void nullSchemeChanged();
        void thicknessBufferChanged();
        void maxThicknessAllowedChanged();
        void foldDecimateChanged();
        void fatStepChanged();
        void closeDipChanged();
        void useInterpolationsChanged();
        void useFatChanged();

    public Q_SLOTS:

    public:
        M2lConfig();
        void reset(void);
        int saveToFile(QString filename = "");
        int loadFromFile(QString filename = "");
//        Q_INVOKABLE void clearProject(bool clearExtents);

        int m_quietMode;
        QString m_deposits;
        QString m_dtb;
        int m_orientationDecimate;
        int m_contactDecimate;
        int m_intrusionMode;
        int m_interpolationSpacing;
        int m_misorientation;
        QString m_interpolationScheme;
        int m_faultDecimate;
        double m_minFaultLength;
        double m_faultDip;
        double m_plutonDip;
        QString m_plutonForm;
        double m_distBuffer;
        double m_contactDip;
        int m_contactOrientationDecimate;
        QString m_nullScheme;
        double m_thicknessBuffer;
        double m_maxThicknessAllowed;
        int m_foldDecimate;
        double m_fatStep;
        double m_closeDip;
        bool m_useInterpolations;
        bool m_useFat;
};

#endif // M2LCONFIG_H
