import { useState } from 'react';
import QRCode from 'qrcode';
import { IconShare2, IconCopy, IconCheck } from '@tabler/icons-react';
import { Popover, PopoverContent, PopoverTrigger } from './ui/popover';
import { Button } from './ui/button';
import { useI18nContext } from '../i18n/i18n-react';

interface ShareButtonProps {
  /** Flushes any uncommitted state (e.g. mid-drag slider values) into the URL. */
  onBeforeShare?: () => void;
}

export const ShareButton = ({ onBeforeShare }: ShareButtonProps) => {
  const { LL } = useI18nContext();
  const [url, setUrl] = useState('');
  const [qrDataUrl, setQrDataUrl] = useState('');
  const [copied, setCopied] = useState(false);

  const handleOpenChange = (open: boolean) => {
    if (!open) {
      setCopied(false);
      return;
    }
    onBeforeShare?.();
    const href = window.location.href;
    setUrl(href);
    QRCode.toDataURL(href, { width: 176, margin: 1 })
      .then(setQrDataUrl)
      .catch(() => { setQrDataUrl(''); });
  };

  const handleCopy = () => {
    void navigator.clipboard.writeText(url).then(() => {
      setCopied(true);
      setTimeout(() => { setCopied(false); }, 1500);
    });
  };

  return (
    <Popover onOpenChange={handleOpenChange}>
      <div
        style={{
          backgroundColor: 'rgba(0, 0, 0, 0.45)',
          borderRadius: '4px',
          padding: '3px',
          pointerEvents: 'auto',
        }}
        onPointerDown={e => { e.stopPropagation(); }}
      >
        <PopoverTrigger
          aria-label={LL.share.share()}
          title={LL.share.share()}
          style={{
            background: 'transparent',
            border: 'none',
            color: 'white',
            padding: '4px',
            cursor: 'pointer',
            borderRadius: '3px',
            lineHeight: 0,
          }}
        >
          <IconShare2 size={18} />
        </PopoverTrigger>
      </div>
      <PopoverContent align="end" className="w-56">
        {qrDataUrl !== '' && (
          <img
            src={qrDataUrl}
            alt={LL.share.scanHint()}
            title={LL.share.scanHint()}
            className="mx-auto size-44 rounded-sm bg-white"
          />
        )}
        <p className="break-all font-mono text-[10px] leading-tight text-muted-foreground">
          {url}
        </p>
        <Button size="sm" onClick={handleCopy} className="w-full">
          {copied ? <IconCheck size={14} /> : <IconCopy size={14} />}
          {copied ? LL.share.copied() : LL.share.copyLink()}
        </Button>
      </PopoverContent>
    </Popover>
  );
};
